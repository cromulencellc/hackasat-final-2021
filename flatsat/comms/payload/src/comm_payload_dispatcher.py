#!/usr/bin/python3
import asyncio
from typing import MutableSequence

from yaml.tokens import DirectiveToken
from gmqtt import Client as MQTTClient
import json
import logging, logging.config

import subprocess, os, binascii, time
import signal
import struct
import yaml
import argparse
import socket
import sys

from enum import IntEnum, Enum

from gmqtt.mqtt.constants import MQTTv311

def setup_logging(cfg_file='logging.yaml'):
    try:
        with open(cfg_file, 'r') as logcfg:
            config = yaml.safe_load(logcfg)
        logging.config.dictConfig(config)
    except:
        print("Using basic config instead")
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s.%(msecs)03d %(levelname)s:\t%(message)s',
            datefmt='%Y-%m-%d %H:%M:%S')
        logging.getLogger("gmqtt").setLevel(logging.WARNING)
    return logging.getLogger("root")

logger = setup_logging()

mqtt_user = os.getenv('MQTT_USER', "comm")
mqtt_pass = os.getenv("MQTT_PASS", "2B5458962584FE417B2803CF")
mqtt_hostname = os.getenv("MQTT_HOST", "192.168.101.101")
mqtt_port = int(os.getenv("MQTT_PORT", "1883"), base=10)
TEAM_NUMBER = int(os.getenv("TEAM_NUMBER", "0"), base=10)

NUM_TEAMS = 8

STATUS_TOPIC       = "COMM/PAYLOAD/SLA"
SLA_TOPIC          = "COMM/PING/STATUS"
SIM_TOPIC          = "SIM/DATA/JSON"
PWR_OVERRIDE_TOPIC = "SIM/PAYLOAD/PWR"

STOP = asyncio.Event()

class DispatcherMode(Enum):
    NORMAL=0
    CHALLENGE4=1

class Destination(bytes):
    COMM    = b'1'
    SLA_TLM = b'2'
    DANX    = b'3'

class PayloadState(IntEnum):
    OFF = 0
    ON = 1

class PayloadPointingState(Enum):
    PAYLOAD_POINTING_ERROR = False
    PAYLOAD_POINTING_SUCCESS = True

global PAYLOAD_STATE
PAYLOAD_STATE = PayloadState.ON
MODE=DispatcherMode.NORMAL
PAYLOAD_POINTING_LINK_STATE=PayloadPointingState.PAYLOAD_POINTING_ERROR

class MqttTransport:

    def __init__(self, logger, client_id, mqtt_host, mqtt_port, mqtt_user, mqtt_pass, mqtt_qos, sim_data_topic, reconnect_interval=5):
        self.logger = logger
        self.host = mqtt_host
        self.port = mqtt_port
        self.user = mqtt_user
        self.password = mqtt_pass
        self.qos = mqtt_qos
        self.sub_topic = sim_data_topic
        self.client_id = client_id
        self.reconnect = reconnect_interval


    async def connect(self):
        self.logger.info(f"Connecting to {self.host}:{self.port} with {self.user}/{self.password}")
        self.client = MQTTClient(self.client_id )
        self.client.set_auth_credentials(self.user, self.password)
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.on_disconnect = self.on_disconnect
        self.client.on_subscribe = self.on_subscribe
        self.client.set_config({'reconnect_retries': 10, 'reconnect_delay': self.reconnect})
        await self.client.connect(self.host, self.port, keepalive=300, version=MQTTv311)


    def on_connect(self, client, flags, rc, properties):
        self.logger.info('MQTT connection successful')
        client.subscribe(self.sub_topic, qos=self.qos)
        client.subscribe(PWR_OVERRIDE_TOPIC, qos=self.qos)

    # Handle received MQTT messages by topic
    def on_message(self, client, topic, payload, qos, properties):
        global PAYLOAD_STATE, PAYLOAD_POINTING_LINK_STATE

        self.logger.debug('RECV MSG:', payload)
        payload_pointing_state=None
        payload_state=None
        off_pointing_angle=None

        if topic == self.sub_topic:
            if len(payload) > 0:
                sim_data = json.loads(payload)
                # print(sim_data)
                payload_state = PayloadState(int(sim_data.get("SC[0].AC.EPS[0].PAYLOAD_Switch", "0"), 10))
                payload_pointing_state = PayloadPointingState(sim_data.get("payload_pointing_state"))
                off_pointing_angle = sim_data.get("payload_offpoint_angle")
        elif topic == PWR_OVERRIDE_TOPIC:
            payload_state = PayloadState(int(payload, 10))
        if payload_state is not None and payload_state != PAYLOAD_STATE:
            try:
                PAYLOAD_STATE = PayloadState(payload_state)
                if PAYLOAD_STATE is not None:
                    self.logger.info(f"PAYLOAD PWR State Changed to {PAYLOAD_STATE.name}")
            except ValueError as e:
                self.logger.error(f"Got {str(e)} while trying to set payload state")
                self.logger.info(f"PAYLOAD_STATE={PAYLOAD_STATE} | payload_state={payload_state} | topic={topic}")
                pass
        elif payload_state is None:
            self.logger.info(f"PAYLOAD_STATE={PAYLOAD_STATE} | payload_state={payload_state} | topic={topic}")

        logger.debug(f"Payload State: PWR: {PAYLOAD_STATE.name}, Pointing: {payload_pointing_state}:{off_pointing_angle}")

        if PAYLOAD_POINTING_LINK_STATE is not None and payload_pointing_state != PAYLOAD_POINTING_LINK_STATE:
            self.logger.info(f"PAYLOAD Pointing State Changed. From: {PAYLOAD_POINTING_LINK_STATE.name}, To: {payload_pointing_state.name}")
            PAYLOAD_POINTING_LINK_STATE = payload_pointing_state

    def on_disconnect(self, client, packet, exc=None):
        self.logger.warning(f"MQTT disconnected")

    def on_subscribe(self, client, mid, qos, properties):
        self.logger.info(f"Subscribed to {mid}")

    def publish(self, topic: str, payload: str, qos: int):
        self.client.publish(topic, payload, qos)

class RadioPacketProtocol:
    def __init__(self, mqtt_client: MqttTransport, logger):
        self.mqtt_client = mqtt_client
        self.logger = logger
        self.seq_pings_received = 0
        self.total_pings_received = 0
        self.round_packet_seq = -1
        self.cur_round = 1

    def connection_made(self, transport):
        self.transport = transport

    def error_received(self, exc):
        self.logger.error('Error received:', exc)

    def connection_lost(self, exc):
        self.logger.warning("Radio UDP onnection closed")

    # Drop in replacement code for handleing received UDP radio packets
    def datagram_received(self, payload, addr):
        global PAYLOAD_STATE
        self.logger.debug(f"Received {payload} from {addr} with payload state {PAYLOAD_STATE.name}")

        # Check if payload is on and payload is pointed to get good link state
        if PAYLOAD_STATE == PayloadState.ON:
            dest = b'0'
            team = b'0'
            if len(payload) > 1:
                dest, team = struct.unpack("cc", payload[:2])
            else:
                logger.warning("Recieved radio packet with len < 2. Ignoring")

            try:
                team = int(team)
            except ValueError as e:
                logger.error("Team is not a literal number. This is not good")

            topic = None

            payload = payload[2:]

            if dest == Destination.COMM:
                if team < 1 or team > NUM_TEAMS:
                    self.logger.error(f"!!BAD!! Destination:COMM | Team Num not between 1 and {NUM_TEAMS}. IGNORING")
                    return
                topic = STATUS_TOPIC
            elif dest == Destination.SLA_TLM:
                if team != 0:
                    self.logger.error(f"!!BAD!! Destination:SLA_TLM | Team Num not 0. IGNORING")
                    return
                topic = SLA_TOPIC
            elif dest == Destination.DANX:
                if team < 1 or team > NUM_TEAMS:
                    self.logger.error(f"!!BAD!! Destination:DANX Service | Team Num not between 1 and {NUM_TEAMS}. IGNORING")
                    return
                topic = b"DANX"
            else:
                self.logger.error(f"!!BAD!! Invalid destination: {dest}")
                return

            self.logger.info(f"Mode: {MODE.name}, Dest: {dest}, Team: {team} Payload: {payload}")

            if topic == SLA_TOPIC:
                if len(payload) < 12:
                    self.logger.error(f">>>>ERROR: too short packet")
                    return

                self.logger.info(f"Received message: {binascii.hexlify(payload)} from addr {addr}")

                sync_word, payload_round_num, payload_seq_num = struct.unpack("<4sBH", payload[:7])
                _, ping_packet = struct.unpack("<4sQ", payload[:12])

                self.logger.info(f"sync: {sync_word} round: {payload_round_num} sequence: {payload_seq_num} ping: {ping_packet}")

                if payload_round_num != self.cur_round and self.round_packet_seq != -1:
                    self.logger.info(f"Round changed from {self.cur_round} to {payload_round_num}")
                    self.cur_round = payload_round_num
                    self.logger.info(f"Prev round seq_pings_received:{self.seq_pings_received} Packet loss:{1-self.seq_pings_received/self.round_packet_seq}")
                    self.seq_pings_received = 0
                    self.round_packet_seq = 1

                self.seq_pings_received += 1
                self.total_pings_received += 1
                self.round_packet_seq += 1

                if self.round_packet_seq != payload_seq_num:
                    self.logger.error(f">>>>>ERROR packet sequence mismatch!: round_packet_seq:{self.round_packet_seq} payload_seq_num:{payload_seq_num}")
                    self.round_packet_seq = payload_seq_num
            elif topic == STATUS_TOPIC:
                pl_len = len(payload)

                self.logger.info(f"Received message: {binascii.hexlify(payload)} from addr {addr}")

                if pl_len < 5:
                    self.logger.error(f">>>>ERROR: too short packet")
                    return

            # Publish the message on mqtt
            if topic == STATUS_TOPIC or topic == SLA_TOPIC:
                self.mqtt_client.publish(topic, payload, qos=2)
            elif topic == b"DANX":
                if team != TEAM_NUMBER:
                    # Send data to the 8 UDP challenge servers (if they exist)
                    self.logger.info("Sending payload to 'DANX' service")
                    send_data_to_challenge_sockets(team, payload)

        else:
            self.logger.debug("Payload currently in OFF or not pointed correctly")

def ask_exit(*args):
    STOP.set()

challenge_sockets = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
def send_data_to_challenge_sockets(team, data):
    addr = ('172.17.0.1', 4530+team)
    challenge_sockets.sendto(data, addr)

async def serve_tcp_server(server):
    async with server:
        await server.serve_forever()

async def main(loop):

    PORT=2303
    mqtt_client = MqttTransport(logger, "PayloadDispatcher", mqtt_hostname, mqtt_port, mqtt_user, mqtt_pass, mqtt_qos=2, sim_data_topic=SIM_TOPIC)

    try:
        await mqtt_client.connect()
        transport, protocol = await loop.create_datagram_endpoint(
            lambda: RadioPacketProtocol(mqtt_client, logger),
            local_addr=('0.0.0.0', PORT))
        await STOP.wait()
        await mqtt_client.client.disconnect()
    finally:
        transport.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-m", "--mode", help="Dispatcher mode (NORMAL or CHALLENGE4)")

    args = parser.parse_args()

    if args.mode is not None:
        MODE=DispatcherMode[args.mode.strip().upper()]

    logger.info(f"Starting Payload Dispatcher with Mode={MODE}")

    loop = asyncio.get_event_loop()
    loop.add_signal_handler(signal.SIGINT, ask_exit)
    loop.add_signal_handler(signal.SIGTERM, ask_exit)
    loop.add_signal_handler(signal.SIGHUP, ask_exit)
    loop.run_until_complete(main(loop))



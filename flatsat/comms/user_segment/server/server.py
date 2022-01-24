#!/usr/bin/env python3

# Cromulence
# Author: meenmachine
# User segment server. This recieved messages from the client, validates signatures,
# and forwards messages to the radio.

import argparse
import asyncio
import logging
import json
import os
from typing import Type
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import socket
import struct
import sys
import yaml
import uuid
from datetime import datetime
from secrets import token_bytes
import subprocess
import ctypes

import psycopg2

from asyncio.base_events import Server
from binascii            import hexlify
from construct           import *
from Crypto.PublicKey    import RSA
from Crypto.Signature    import pkcs1_15
from Crypto.Hash         import SHA256
from ctypes              import c_uint8, sizeof, c_uint16, c_uint32, c_uint64
from errno               import EACCES, EADDRINUSE, EADDRNOTAVAIL
from time                import time_ns, sleep

from dataclasses         import dataclass

# Local imports
from tools.generate_team_keys_v2 import RSA_BITS, RSA_BYTES, gen_keys_client
from tools.generate_team_keys_safe import gen_keys_good
from tools.ping_controller.controller import ROUNDS_PER_GAME, check_pings_dir

PATH_TO_CONFIG = f"{os.getcwd()}/configs"
KEYGEN_PATH    = f"{os.getcwd()}/tools/key_gen"

# Conversions
NSEC_PER_MSEC   = 1E6 # how many microseconds in a millisecond
NSEC_PER_SEC    = 1E9 # how many nanoseconds in a second

# Game Settings
NUM_TEAMS        = 8
TIMEOUT_STD_MSG  = 1     # In seconds
TIMEOUT_TEAM     = 1   # In seconds
MAX_MSG_SIZE     = 240
DISPATCH_IP      = 'localhost' #'192.168.2.35'

# Global Arguments (Can change from argparse)
DISPATCH_SEND   = False
MQTT_SEND       = False  # Should we send messages through MQTT
SOCKET_SEND     = False  # Should we send messages through the radio
USER_SEG_PORT   = 31337
USER_SEG_ADDR   = '127.0.0.1'

# MQTT Settings
MQTT_TOPIC      = "COMM/PAYLOAD/SLA"
MQTT_ADDR       = '127.0.0.1'
MQTT_PORT       = 1883
COMM_USER_PASS  = {'username': 'comm',
                   'password': '2B5458962584FE417B2803CF'}

# Globals
TIMEOUT_STD_MSG_NS = TIMEOUT_STD_MSG * NSEC_PER_SEC
TIMEOUT_TEAM_NS    = TIMEOUT_TEAM * NSEC_PER_SEC

MESSAGE_HEADER_LEN = RSA_BYTES + sizeof(c_uint8) + sizeof(c_uint32) + sizeof(c_uint64) + sizeof(c_uint64) + sizeof(c_uint16)

TEAM_WINNERS = {1: False,
                2: False,
                3: False,
                4: False,
                5: False,
                6: False,
                7: False,
                8: False}

# Team RSA-2048 pub key list <ID>:<RsaKey>
TEAM_KEY_LOOKUP = {1: None,
                   2: None,
                   3: None,
                   4: None,
                   5: None,
                   6: None,
                   7: None,
                   8: None}

# Team Attribution Keys <ATTRIBUTION_KEY>:<TEAM_NUM>
# These are loaded from team_attr_keys.yml
TEAM_ATTR_KEY_LOOKUP = {}

TEAM_IP_TO_ATTR_KEY  = {
    '10.0.0.1': 1,
    '10.0.0.2': 2,
    '10.0.0.3': 3,
    '10.0.0.4': 4,
    '10.0.0.5': 5,
    '10.0.0.6': 6,
    '10.0.0.7': 7,
    '10.0.0.8': 8
}

# Cached binary public keys for faster key lookup for teams
TEAM_DER_KEY_LOOKUP = {1: b'',
                       2: b'',
                       3: b'',
                       4: b'',
                       5: b'',
                       6: b'',
                       7: b'',
                       8: b''}

TEAM_PORT_LOOKUP = {1: 3001,
                    2: 3002,
                    3: 3003,
                    4: 3004,
                    5: 3005,
                    6: 3006,
                    7: 3007,
                    8: 3008}

PING_GEN_SERVER_IP       = '127.0.0.1'
PING_GEN_SERVER_PORT     = 3099
PING_GEN_SERVER_TEAM_NUM = 99

# RSA_BITS/BYTES defined in generate_team_keys
CommPayloadMonitoringPacket = Struct(
    'signature' / Bytes( RSA_BYTES ),
    'serviced'  / Flag,
    'sync'      / Bytes(4),
    'id'        / Int64ul,
    'key'       / Int64ul,
    'datalen'   / Int16ul
)

CommPingPacket = Struct(
    'destination' / Bytes(1),
    'team_num'    / Bytes(1),
    'sync'        / Bytes(4),
    'ping'        / Int64ul
)

LOG_LEVEL   = logging.ERROR
LOG_FOLDER  = "logs"
LOG_TO_FILE = True

loggerFormat = '%(asctime)s %(levelname)s %(message)s'

# DATABASE_URL = os.getenv('DATABASE_URL',
#                          "postgresql://removed@localhost/has2")
# DATABASE_CONNECTION = psycopg2.connect(DATABASE_URL)
# DATABASE_CONNECTION.autocommit = True

### Logfile generation ###
if not(os.path.exists(LOG_FOLDER) and os.path.isdir(LOG_FOLDER)):
    try:
        os.mkdir(LOG_FOLDER)
    except Exception as e:
        sys.stdout.write(f"(ERROR) Log folder `{LOG_FOLDER}` doesn't exist and can't be made. Got: {str(e)}\n")
        sys.stdout.write("Will only log to stdout.")
        sys.stdout.flush()
        LOG_FOLDER = ""
        LOG_TO_FILE = False

gen_log_name = lambda : f"server_{str(uuid.uuid4())[:8]}_{datetime.now().strftime('%m-%d_%H-%M')}"

logger = logging.getLogger(__name__)

log_name = None
if LOG_TO_FILE:
    log_name = f"{LOG_FOLDER}/{gen_log_name()}.log"
    while(os.path.exists(log_name)):
        log_name = f"{LOG_FOLDER}/{gen_log_name()}.log"

logging.basicConfig(format=loggerFormat,
                    handlers=[
                        logging.StreamHandler(sys.stdout),
                        logging.FileHandler(log_name)
                        ],
                    level=LOG_LEVEL)

class Destination(Enum):
    COMM    = b'1'
    SLA_TLM = b'2'
    DANX    = b'3'

class SyncWords(Enum):
    COMM    = b'AAAA'
    SLA_TLM = b'BBBB'
    DANX    = b'CCCC'

class UserSegmentProtocol(asyncio.Protocol):
    def __init__(self, loop, teams):
        self.transport      = None
        self.loop           = loop
        self.teams          = teams
        self.team_ip        = ''

    def connection_made(self, transport):
        peername = transport.get_extra_info('peername') # Get the IP and port we just connected to

        logger.info(f"Made new connection {peername[0]} : {peername[1]}\n")
        self.team_ip = peername[0]

        self.transport = transport

        return

    def data_received(self, data):
        logger.debug("Recieved data")
        
        while len(data) > 0:
            if len(data) >= MESSAGE_HEADER_LEN:
                team_num = -1

                try:
                    msgHeader = CommPayloadMonitoringPacket.parse(data[0:MESSAGE_HEADER_LEN])
                except StreamError as e:
                    logger.warning(f"Error while trying to parse message from interface: {str(e)}")
                    self.transport.write(Team.WRONG_PACK_SIZE)
                    return
                except TypeError as e:
                    logger.error(f"(BAD) Error while trying to parse message from interface: {str(e)}")
                    return

                team_id       = msgHeader.id
                team_attr_key = msgHeader.key
                serviced      = msgHeader.serviced

                msg_with_head = data[RSA_BYTES : (MESSAGE_HEADER_LEN + msgHeader.datalen)]
                msg           = data[MESSAGE_HEADER_LEN : (MESSAGE_HEADER_LEN + msgHeader.datalen)]
                data          = data[(MESSAGE_HEADER_LEN + msgHeader.datalen) : ]

                logger.debug(msgHeader)

                team_id_from_attr_key = 0
                ### TODO: These three checks should move ABOVE parsing to make things faster during the game.
                if not team_attr_key in TEAM_ATTR_KEY_LOOKUP:
                    logger.debug("Team sent message with an invalid attribution key")
                    self.transport.write(Team.ATTR_NOT_FOUND)
                    return
                else:
                    team_id_from_attr_key = TEAM_ATTR_KEY_LOOKUP[team_attr_key]
                    
                if self.team_ip not in TEAM_IP_TO_ATTR_KEY:
                    self.transport.write(Team.UNKNOWN_IP)
                    return
                elif TEAM_IP_TO_ATTR_KEY[self.team_ip] != TEAM_ATTR_KEY_LOOKUP[team_attr_key]:
                    logger.info(f"{TEAM_IP_TO_ATTR_KEY[self.team_ip]} != {TEAM_ATTR_KEY_LOOKUP[team_attr_key]}")
                    self.transport.write(Team.ATTR_KEY_DOES_NOT_MATCH_IP)
                    return

                if (team_id < 1 or team_id > NUM_TEAMS) and team_id != 0xDEB06FFFFFFFFFFF:
                    logger.debug("Team sent message with a team ID that doesn't exist")
                    self.transport.write(Team.NO_ID_FOUND)
                    return
                if(len(msg) > MAX_MSG_SIZE):
                    logger.debug(f"Team sent message with length greater than {MAX_MSG_SIZE}")
                    self.transport.write(Team.MSG_TOO_LARGE)
                    return
                if(len(msg) < msgHeader.datalen):
                    logger.warning(f"Received packet with larger data_len than length of data sent. | Got {len(msg)} | Expected {msgHeader.datalen} \nIGNORING")
                    self.transport.write(Team.LEN_NO_MATCH)
                    return

                # TODO: Guess the team with the team ID first to potentially go a bit faster
                verified_team_num = -1
                for team_num in range(1,NUM_TEAMS+1):
                    try:
                        digest = SHA256.new(msg_with_head)
                        pkcs1_15.new(TEAM_KEY_LOOKUP[team_num]).verify(digest, msgHeader.signature)
                        verified_team_num = team_num
                        break
                    except ValueError as e:
                        pass
                    except Exception as e:
                        logger.error(f"(BAD) While attempting to verify signature for digest: TEAM {team_num} | Error: {str(e)}")

                if(verified_team_num == -1):
                    logger.debug("Could not verify message to a team")
                    self.transport.write(Team.WRONG_KEY)
                    continue

                if verified_team_num != team_id_from_attr_key:
                    ### Commented out as part of release
                    # try:
                    #     with DATABASE_CONNECTION.cursor() as cur:
                    #         cur.execute("""
                    #         INSERT INTO overwrites
                    #         (service_id, overwriting_team_id, clobbered_team_id,
                    #         inserted_at, updated_at)
                    #         values (%s, %s, %s, now(), now());""",
                    #                     (3, team_id_from_attr_key, verified_team_num))
                    # except Exception as e: # Don't do this at home kids
                    #     logger.error(f"Could not update Scoring DB. Got {str(e)}")
                    
                    sys.stdout.write(f"!!!!!!! WINNER: Team {team_id_from_attr_key} is pwning Team {verified_team_num}!!!!!!!\n")
                    sys.stdout.flush()
                    logger.info(f"!!!!!!! WINNER: Team {team_id_from_attr_key} is pwning Team {verified_team_num}!!!!!!!")

                logger.debug(f"TEAM {verified_team_num} verified message | Team_id: {team_id}")

                if team_id == 0xDEB06FFFFFFFFFFF:
                    self.loop.create_task((self.teams[team_id_from_attr_key-1].send_pub_key(msgHeader,
                                                                                            msg,
                                                                                            self.transport)))
                    continue

                if verified_team_num <= NUM_TEAMS and verified_team_num > 0:
                    if team_id != verified_team_num:
                        logger.debug(f"Message sent with Team {verified_team_num}'s signature, but team {team_id}'s id. Ignoring")
                        self.transport.write(Team.MISMATCHED)
                        continue

                    self.loop.create_task((self.teams[team_id_from_attr_key-1].handle_msg(serviced,
                                                                                          msg,
                                                                                          self.teams[verified_team_num-1],
                                                                                          self.transport)))
                else:
                    logger.error("SOMETHING VERY BAD IS HAPPENING. There are more teams than expected. PLEASE CHECK")

                logger.debug(f"End: {len(data)}")
            else:
                logger.warning(f"Received too small of a message: {hexlify(data)} | Got {len(data)} | Expected: {MESSAGE_HEADER_LEN}")
                data = b''
                break

    def connection_lost(self, exc):
        logger.info('The client closed the connection')

class RadioClientProtocol:
    def __init__(self, on_con_lost):
        self.on_con_lost = on_con_lost
        self.transport = None

    def connection_made(self, transport):
        self.transport = transport

    def datagram_received(self, data, addr):
        logger.info(f"Received from radio: {data}")

    def error_received(self, exc):
        logger.error(f"RadioClient got error: {str(exc)}")

    def connection_lost(self, exc):
        logger.critical("USER SEGMENT LOST CONNECTION TO RADIO")
        logger.critical("CONNECTION CLOSED")
        self.on_con_lost.set_result(True)

class Team():

    # Add constants for Data Type as well
    PACKET_SUCCESS  = b'SUCCESS'
    LEN_NO_MATCH    = b'BAD_LENGTH'
    WRONG_PACK_SIZE = b'WRONG_PACK_SIZE'
    MSG_TOO_SOON    = b'RATE_LIMIT'
    WRONG_KEY       = b'CANT_DECODE_WRONG_KEY'
    NO_ID_FOUND     = b'ID_NOT_FOUND'
    BAD_MSG         = b'BAD_MSG'
    MISMATCHED      = b'MISMATCHED_TEAM_ID_BUT_VALID_KEY'
    ATTR_NOT_FOUND  = b'ATTR_KEY_NOT_FOUND'
    MSG_TOO_LARGE   = b'MSG_TOO_LARGE'
    UNKNOWN_IP      = b'UNKNOWN_IP'
    ATTR_KEY_DOES_NOT_MATCH_IP = b'ATTR_KEY_DOES_NOT_MATCH_IP'

    # ping lookup table and locks
    ping_on         = True
    ping_lookup     = {}
    ping_lock       = asyncio.Lock()
    cur_ping_seq    = 1
    cur_ping_round  = 1
    last_round      = 1

    def __init__(self, team_num, loop, radio_transport=None, radio_protocol=None):
        self.team_num         = team_num
        self.loop             = loop
        self.radio_transport  = radio_transport
        self.radio_protocol   = radio_protocol
        self.lock             = asyncio.Lock()
        self.can_send_msg     = True
        self.last_packet_time = time_ns()

        # TODO: Potentially use password from settings file
        self.client_id = f"user_seg_t{team_num}"

        self.loop.create_task(self.team_message_timer())
        self.loop.create_task(self.standard_packet_timer())

    async def handle_msg(self, serviced, msg, team_to_send_to, transport):

        # Make sure data sent is as long as datalen
        logger.debug("Handling msg | ")

        async with self.lock:
            if self.can_send_msg == False:
                logger.debug(f"DID NOT SEND -- Too soon to send message | time_ns: {time_ns()} | last_packet_time {self.last_packet_time}")
                transport.write(Team.MSG_TOO_SOON)
                return
            else:
                self.can_send_msg = False
                logger.debug("Sending msg | ")
                self.last_packet_time = time_ns()

        if(MQTT_SEND):
            publish.single("COMM/PAYLOAD/USERREQ",
                        payload=msg,
                        qos=2,
                        hostname=MQTT_ADDR,
                        port=MQTT_PORT,
                        client_id=self.client_id,
                        auth=COMM_USER_PASS)
        elif(DISPATCH_SEND):
            # self.team_num in this case is actually the team who this messaged is being attributed to.
            # The team sat we are sending the msg to and the team who sends that message may not be equal

            payload = (b'3' if serviced else b'1') + str(self.team_num).encode() + msg

            logger.debug(f"Sending to team {team_to_send_to.team_num} radio at: ({DISPATCH_IP}, {TEAM_PORT_LOOKUP[team_to_send_to.team_num]})|\n{payload}")

            team_to_send_to.radio_transport.sendto(payload)
        else:
            logger.error("SOMETHING VERY BAD IS HAPPENING. Both MQTT_SEND and DISPATCH_SEND are OFF. PLEASE CHECK")

        transport.write(Team.PACKET_SUCCESS)
        return

    # Sends back public keys to teams if they figure out challenge
    async def send_pub_key(self, msgHeader, msg, transport):
        try:
            if msgHeader.datalen < 2:
                team_num = ord(msg)
            elif msgHeader.datalen <= 8:    # Way slower, avoid if possible
                team_num = struct.unpack('>Q', (b'\x00' * (8-len(msg)))+msg)[0]
            else:
                raise TypeError("Team number larger than 8 bytes")

            logger.debug(f"!!!!!!! PUB KEY FOUND {self.team_num} asked to send team {team_num}'s public key !!!!!!!")

            if team_num in TEAM_DER_KEY_LOOKUP:
                transport.write(TEAM_DER_KEY_LOOKUP[team_num])
                logger.info(f"!!!!!!! PUB KEY FOUND | Sending team {self.team_num} public key for {team_num} !!!!!!!")
            else:
                logger.debug(f"Team {team_num}'s key doesn't exist. IGNORING...")
                transport.write(Team.NO_ID_FOUND)
        except ValueError:
            transport.write(Team.BAD_MSG)
        except TypeError as e:
            logger.error(f"(BAD) send_pub_key got {str(e)}")
            transport.write(Team.NO_ID_FOUND)
        except struct.error as e:
            logger.error(f"(BAD) send_pub_key got {str(e)}")
            transport.write(Team.NO_ID_FOUND)

    async def standard_packet_timer(self):
        payload  = b""
        cur_ping = b""

        while True:
            async with Team.ping_lock:
                if not Team.ping_on:
                    logger.info(f"Team {self.team_num} pinger logging off.")
                    return
                elif Team.cur_ping_seq in Team.ping_lookup[Team.cur_ping_round]:
                    cur_ping = Team.ping_lookup[Team.cur_ping_round][Team.cur_ping_seq]

            payload = CommPingPacket.build(dict(destination=Destination.SLA_TLM, team_num=b'0', sync=SyncWords.SLA_TLM, ping=cur_ping))

            if(MQTT_SEND):
                publish.single(MQTT_TOPIC,
                            payload=payload,
                            qos=2,
                            hostname=MQTT_ADDR,
                            port=MQTT_PORT,
                            client_id=self.client_id,
                            auth=COMM_USER_PASS)
            if(DISPATCH_SEND):
                self.radio_transport.sendto(payload)

            await asyncio.sleep(TIMEOUT_STD_MSG)

    async def team_message_timer(self):
        # Check what team you're on first
        t = 0
        while True:
            async with self.lock:
                t = time_ns() - self.last_packet_time
                if t > TIMEOUT_TEAM_NS:
                    self.can_send_msg = True
            await asyncio.sleep( TIMEOUT_TEAM_NS/NSEC_PER_SEC if t > TIMEOUT_TEAM_NS else (TIMEOUT_TEAM_NS - t)/NSEC_PER_SEC )

class PingController():
    def __init__(self, team_num, loop, radio_transport=None, radio_protocol=None):
        self.team_num         = team_num
        self.loop             = loop
        self.radio_transport  = radio_transport
        self.radio_protocol   = radio_protocol
        self.lock             = asyncio.Lock()
        self.can_send_msg     = False
        self.last_packet_time = time_ns()

        self.loop.create_task(self.standard_packet_timer())

    async def standard_packet_timer(self):
        payload  = b""
        cur_ping = b""
        await asyncio.sleep(TIMEOUT_STD_MSG/2)
        while True:
            async with Team.ping_lock:
                if Team.cur_ping_seq + 1 in Team.ping_lookup[Team.cur_ping_round]:
                    Team.cur_ping_seq += 1
                    cur_ping = Team.ping_lookup[Team.cur_ping_round][Team.cur_ping_seq]
                elif Team.last_round <= Team.cur_ping_round:
                    logger.warning("\n\n\n\nEND OF GAME!!!\n\n\n\n")
                    Team.ping_on = False
                    return
                else:
                    Team.cur_ping_round += 1
                    Team.cur_ping_seq    = 1
                    logger.warning(f"\n\n\n\nMoving onto round {Team.cur_ping_round}\n\n\n\n\n\n")

            payload = CommPingPacket.build(dict(destination=Destination.SLA_TLM, team_num=b'0', sync=SyncWords.SLA_TLM, ping=cur_ping))
            if(MQTT_SEND):
                publish.single(MQTT_TOPIC,
                            payload=payload,
                            qos=2,
                            hostname=MQTT_ADDR,
                            port=MQTT_PORT,
                            client_id=self.client_id,
                            auth=COMM_USER_PASS)
            if(DISPATCH_SEND):
                self.radio_transport.sendto(payload)

            await asyncio.sleep(TIMEOUT_STD_MSG)

async def inverse_dict(h):
    return {v: k for k, v in h.items()}

async def check_radio_up(team_num, radio_addr=DISPATCH_IP, special_player=False):
    if not special_player and team_num not in TEAM_PORT_LOOKUP:
        logger.fatal("Either the number of teams is incorrect or TEAM_PORT_LOOKUP is improperly defined.")
        logger.fatal("Bailing out...")
        exit(-1)

    ping_port = subprocess.Popen(["nc",
                                  "-vuzw",
                                  "1",
                                  f"{radio_addr}",
                                  str(TEAM_PORT_LOOKUP[team_num]) if not special_player else str(PING_GEN_SERVER_PORT)
                                  ], stdout=subprocess.PIPE)
    ping_port.communicate() # Wait for it to exit

    if ping_port.returncode != 0 and not special_player:
        logger.fatal("Please check that the radio is up.")
        logger.fatal(f"Nothing is listening on port {TEAM_PORT_LOOKUP[team_num]} for team {team_num}")
        logger.fatal("Or run script with '--nocheck' args (NOT RECOMMENDED)")
        exit(-1)
    elif ping_port.returncode != 0 and special_player:
        logger.fatal("Ping controller not up. Please start that script before continuing...")
        logger.fatal("Or run script with '--nocheck' args (NOT RECOMMENDED)")
        exit(-1)

    return True

async def check_radio_up_bind(team_num, radio_addr=DISPATCH_IP, special_player=False):
    if not special_player and team_num not in TEAM_PORT_LOOKUP:
        logger.fatal("Either the number of teams is incorrect or TEAM_PORT_LOOKUP is improperly defined.")
        logger.fatal("Bailing out...")
        exit(-1)

    check_port = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # This checks to make sure that the radio is bound and listening on the ports we are promised.
    try:
        check_port.bind((radio_addr, TEAM_PORT_LOOKUP[team_num] if not special_player else PING_GEN_SERVER_PORT))
    except socket.error as e:
        if e.errno == EADDRINUSE or e.errno == EADDRNOTAVAIL:   # Good
            return
        elif not special_player:
            check_port.close()
            logger.fatal("Got an unhandled socket error while checking to see if RADIO is up")
            logger.fatal(f"Got: {str(e)}")
            logger.fatal("Bailing out...")
            exit(-1)

    # Fail case
    check_port.close()
    if not special_player:
        logger.fatal("Please check that the radio is up.")
        logger.fatal(f"Nothing is listening on port {TEAM_PORT_LOOKUP[team_num]} for team {team_num}")
    else:
        logger.fatal("Ping Generator Server not up. Please start that script before continuing...")

    logger.fatal("Bailing out...")
    exit(-1)

async def main():
    global MQTT_SEND         # Enable or disable MQTT send. If disabled, will use radio
    global DISPATCH_SEND
    global USER_SEG_PORT
    global USER_SEG_ADDR
    global PATH_TO_CONFIG
    global KEYGEN_PATH
    global DISPATCH_IP

    parser = argparse.ArgumentParser(description="User segment server. Takes user packets from operators passes them onto the MQTT bus")

    parser.add_argument('-p',
                        '--port',
                        metavar='65536',
                        default=USER_SEG_PORT,
                        type=int,
                        help='Port to connect to interface on')
    parser.add_argument('-r',
                        '--address',
                        metavar='localhost',
                        default=USER_SEG_ADDR,
                        type=str,
                        help='Address to connect to interface on')
    parser.add_argument('-m',
                        '--mqtt',
                        action='store_true',
                        help='Use MQTT. Used for DigitalTwin',
                        default=False)
    parser.add_argument('-d',
                        '--dispatch',
                        action='store_true',
                        help='Use direct to dispatch. Used for DigitalTwin',
                        default=False)
    parser.add_argument('-c',
                        "--config",
                        default=PATH_TO_CONFIG,
                        help="Path to config folder")
    parser.add_argument('-k',
                        "--keygen",
                        default=KEYGEN_PATH,
                        help="Path to keygen folder")
    parser.add_argument('-v',
                        '--verbose',
                        action='count',
                        help="# of v's is verbosity level | -v is ERROR -vv is WARNING -vvv is INFO -vvvv is DEBUG | Default is NOTSET",
                        default=0)
    parser.add_argument('--radioaddr',
                        metavar='localhost',
                        default=DISPATCH_IP,
                        type=str,
                        help='Address to connect to the radio interface on')
    parser.add_argument("--generate",
                        help="This will generate all team keys and pings, then exit",
                        action="store_true",
                        default=False)
    parser.add_argument("--goodkeys",
                        help="This will generate RSA keys CORRECTLY",
                        action="store_true",
                        default=False)
    parser.add_argument("-n",
                        "--nocheck",
                        help="Don't bail out if radios/controller aren't listening on their ports",
                        action="store_true",
                        default=False)
    parser.add_argument("-l",
                        "--livegame",
                        help="Run as though it's production. This is the live game, everyone stay calm",
                        action="store_true",
                        default=False)
    parser.add_argument("-o",
                        "--roundnum",
                        help="Start the game from this round number",
                        type=int,
                        default=1)
    parser.add_argument("-s",
                        "--sequence",
                        help="Start the game from this sequence",
                        type=int,
                        default=1)

    args = parser.parse_args()

    DISPATCH_SEND    = args.dispatch
    MQTT_SEND        = args.mqtt and not DISPATCH_SEND
    USER_SEG_PORT    = args.port
    USER_SEG_ADDR    = args.address
    PATH_TO_CONFIG   = args.config
    KEYGEN_PATH      = args.keygen
    DISPATCH_IP      = args.radioaddr

    generate_only = args.generate
    good_keys     = args.goodkeys
    no_check      = args.nocheck
    live_game     = args.livegame
    a_round_num   = args.roundnum   # round num argument
    sequence      = args.sequence


    if live_game:
        sys.stdout.write("!!!RUNNING AS THOUGH ITS THE LIVE GAME!!!\n")
        sys.stdout.flush()
    else:
        sys.stdout.write("NOT running as production (not the live game)\n")
        sys.stdout.flush()

    if not MQTT_SEND and not DISPATCH_SEND and not generate_only:
        logger.warning("Did not choose either MQTT or DISPATCH. User segment will receive messages but not do anything with them")
        logger.warning("You should restart and choose either --mqtt or --dispatch")
        logger.warning("Continuing...")

    sys.stdout.write("It may take up to 20 seconds to start the server...\n")
    sys.stdout.flush()

    # Set logging level
    if(args.verbose == 0):  # Default set this way because of async debug print in help message
        logger.setLevel(logging.NOTSET)
    elif(args.verbose == 1):
        logger.setLevel(logging.ERROR)
    elif(args.verbose == 2):
        logger.setLevel(logging.WARNING)
    elif(args.verbose == 3):
        logger.setLevel(logging.INFO)
    elif(args.verbose == 4):
        logger.setLevel(logging.DEBUG)

    if os.path.exists(f'{PATH_TO_CONFIG}/team_attr_keys.yml'):
        if os.path.isfile(f'{PATH_TO_CONFIG}/team_attr_keys.yml'):
            with open(f'{PATH_TO_CONFIG}/team_attr_keys.yml', 'r') as team_attr_keys_file:
                team_attribution_keys = yaml.safe_load(team_attr_keys_file)

            if (not team_attribution_keys) or (len(team_attribution_keys) < NUM_TEAMS):
                logger.error(f"team_attr_keys.yml does not include all teams 1 through {NUM_TEAMS}. Bailing out...")
                exit(-1)
            for team_num in range(1, NUM_TEAMS+1):
                if not team_num in team_attribution_keys:
                    logger.error(f"team_attr_keys.yml does not include all teams 1 through {NUM_TEAMS}. Bailing out...")

            team_attribution_keys = await inverse_dict(team_attribution_keys) # Reverse key and value

            try:
                TEAM_ATTR_KEY_LOOKUP.update(team_attribution_keys)
            except KeyError as e:
                logger.error("team_attr_keys.yml is incorrectly formatted. Bailing out...")
                exit(-1)
            except Exception as e:
                logger.error(f"Error while trying to parse team_attr_keys.yml file: {str(e)}")
                exit(-1)
        else:
            logger.error(f"{PATH_TO_CONFIG}/team_attr_keys.yml is not a file.")
    else:
        team_attribution_keys = {}
        for team_num in range(1,NUM_TEAMS+1):
            team_attribution_keys[team_num] = struct.unpack("Q", token_bytes(8))[0]
        with open(f"{PATH_TO_CONFIG}/team_attr_keys.yml", 'w') as team_attr_keys_file:
            yaml.safe_dump(team_attribution_keys, team_attr_keys_file)
        team_attribution_keys = await inverse_dict(team_attribution_keys)
        TEAM_ATTR_KEY_LOOKUP.update(team_attribution_keys)

    # Call the team key generate function if a team's keys don't exist
    for team_num in range(1,NUM_TEAMS+1):
        if(not os.path.exists(f"{PATH_TO_CONFIG}/team_public_keys/team_{team_num}_rsa_pub.pem")):
            logger.warning(f"Path to '{PATH_TO_CONFIG}/team_public_keys/team_{team_num}_rsa_pub.pem' doesn't exist...")
            if not good_keys:
                ret = gen_keys_client(NUM_TEAMS, PATH_TO_CONFIG, kg_path="tools/key_gen", gen_new_secrets=False)
                if(not ret):
                    logger.fatal("Unable to generate RSA keys for teams... Bailing out\n")
                    exit(-1)
                break
            else:
                gen_keys_good(NUM_TEAMS, PATH_TO_CONFIG)

    if generate_only:
        for team_num in range(1,NUM_TEAMS+1):
            if(not os.path.exists(f"{PATH_TO_CONFIG}/team_private_keys/team_{team_num}_rsa_priv.pem")):
                logger.warning(f"Path to '{PATH_TO_CONFIG}/team_private_keys/team_{team_num}_rsa_priv.pem' doesn't exist...")
                if not good_keys:
                    ret = gen_keys_client(NUM_TEAMS, PATH_TO_CONFIG, kg_path="tools/key_gen", gen_new_secrets=False)
                    if(not ret):
                        logger.fatal("Unable to generate RSA keys for teams... Bailing out\n")
                        exit(-1)
                    break
                else:
                    gen_keys_good(NUM_TEAMS, PATH_TO_CONFIG)

        sys.stdout.write("Finished generating keys and attribution keys...\n")
        sys.stdout.write("Exiting...\n")
        sys.stdout.flush()
        exit(0)

    # Check if ping packets are created. Otherwise create them
    if live_game:
        await check_pings_dir(f"{PATH_TO_CONFIG}/round_pings")
    else:
        logger.warning("Skipping ping checks (not production)...")

    # Load team public keys obj
    for team_num in range(1,NUM_TEAMS+1):
        with open(f"{PATH_TO_CONFIG}/team_public_keys/team_{team_num}_rsa_pub.pem", 'rb') as pub_key_file:
            key = pub_key_file.read()
            key = RSA.import_key(key)

            TEAM_KEY_LOOKUP[team_num] = key
            TEAM_DER_KEY_LOOKUP[team_num] = key.exportKey("DER")

            key = None

    if MQTT_SEND:
        # Check to make sure that mosquitto is running before start the user_segment server up
        mqtt_client = mqtt.Client("ServerCheck", clean_session=True)
        mqtt_client.username_pw_set(COMM_USER_PASS['username'], COMM_USER_PASS['password'])
        try:
            mqtt_client.connect(MQTT_ADDR, MQTT_PORT)
        except socket.error as e:
            logger.fatal(f"\nERROR: Unable to connect to MQTT instance got: {str(e)}")
            logger.fatal("Please start up MQTT if not already up. Otherwise make sure MQTT host is reachable.")
            logger.fatal("Bailing out...")
            exit(-1)

        mqtt_client.disconnect()

    # No check means don't bail out if radio/controller isn't up
    if not no_check:
        sys.stdout.write(f"Checking to see if controller is up at {DISPATCH_IP}:{PING_GEN_SERVER_PORT}...\n")
        sys.stdout.flush()
        await check_radio_up(PING_GEN_SERVER_PORT, DISPATCH_IP, special_player=True)

        if DISPATCH_SEND:
            sys.stdout.write(f"Checking to see if team radio ports are up at {DISPATCH_IP} : {TEAM_PORT_LOOKUP[1]} - {TEAM_PORT_LOOKUP[NUM_TEAMS]}...\n")
            sys.stdout.flush()
            for team_num in range(1,NUM_TEAMS+1):
                await check_radio_up(team_num)

    sys.stdout.write("LOADING ping files. This could take a few seconds...\n\n")
    sys.stdout.flush()
    async with Team.ping_lock:
        for round_num in range(1, ROUNDS_PER_GAME+1):
            with open(f"{PATH_TO_CONFIG}/round_pings/round_{round_num}.json", 'r') as ping_file:
                ping_lookup_round = json.load(ping_file)
                if not "pings" in ping_lookup_round:
                    logger.critical("Ping file formatted incorrectly. Bailing out...")
                    exit(-1)

                ping_lookup_round["pings"] = {int(k):int(v) for k,v in ping_lookup_round["pings"].items()}
                Team.ping_lookup[round_num] = ping_lookup_round["pings"]

        Team.last_round = max(Team.ping_lookup) # Our last round will be whatever the max number is of all the rounds

        if a_round_num > Team.last_round:
            logger.critical(f"Told to start from a round number that is larger than the last round: {Team.last_round}")
            logger.critical("Bailing out")
            exit(-1)
        elif not a_round_num in Team.ping_lookup:
            logger.critical(f"Told to start from a round number that DOES NOT EXIST")
            logger.critical("Bailing out")
            exit(-1)
        Team.cur_ping_round = a_round_num
        Team.cur_ping_seq   = sequence

    sys.stdout.write(f"Attempting to start on {USER_SEG_ADDR}:{USER_SEG_PORT} with radio at: {DISPATCH_IP}:{TEAM_PORT_LOOKUP[1]}-{TEAM_PORT_LOOKUP[NUM_TEAMS]}\n\n")
    sys.stdout.flush()

    # Start the server up
    loop = asyncio.get_running_loop()

    # Create all the team objects
    teams = []
    teams_udp_con_lost = []
    radio_transport, radio_protocol = None, None
    for team_num in range(1, NUM_TEAMS+1):
        if DISPATCH_SEND:
            teams_udp_con_lost.append(loop.create_future())
            radio_transport, radio_protocol = await loop.create_datagram_endpoint(lambda: RadioClientProtocol(on_con_lost=teams_udp_con_lost[team_num-1]),
                                                                                    remote_addr=(DISPATCH_IP, TEAM_PORT_LOOKUP[team_num]))
        teams.append(Team(team_num, loop, radio_transport, radio_protocol))

    # Ping Scoring Generator "Player"
    teams_udp_con_lost.append(loop.create_future())
    radio_transport, radio_protocol = await loop.create_datagram_endpoint(lambda: RadioClientProtocol(on_con_lost=teams_udp_con_lost[-1]),
                                                                                    remote_addr=(DISPATCH_IP, PING_GEN_SERVER_PORT))
    teams.append(PingController(PING_GEN_SERVER_TEAM_NUM, loop, radio_transport, radio_protocol))

    while True:
        try:
            server = await loop.create_server(
                lambda: UserSegmentProtocol(loop, teams),
                USER_SEG_ADDR, USER_SEG_PORT)
            break
        except socket.error as e:
            logger.error(f"Socket could not be created: {str(e)}")
            if e.errno == EACCES:
                logger.fatal(f"Cannot use privileged port {USER_SEG_PORT}. Use a higher port number...")
                logger.fatal("Bailing out...")
                exit(-1)
            logger.error("Trying again in 3 seconds...")
            sleep(3)

    sys.stdout.write("SUCCESSFUL. GAME ON\n\n")
    sys.stdout.write("### User Segment Server ###\n")
    sys.stdout.flush()
    async with server:
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())

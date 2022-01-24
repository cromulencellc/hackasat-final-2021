import asyncio
from typing import OrderedDict
from asyncio_mqtt import Client, MqttError
from dotenv import dotenv_values
import os
import argparse
import sys
import logging
from numpy.core.records import array
from paho.mqtt.client import MQTTMessage
from messages import IncommingMessage, GameControlMessages, GameControlState

import time
from pyquaternion import Quaternion
import numpy as np
import json


config = dotenv_values(".env")

mqtt_user = os.getenv('MQTT_USER', config["MQTT_USER"])
mqtt_pass = os.getenv("MQTT_PASS", config["MQTT_PASS"])
mqtt_hostname = os.getenv("MQTT_HOST", config["MQTT_HOST"])
mqtt_port = int(os.getenv("MQTT_PORT", config["MQTT_PORT"]), base=10)
mqtt_filter = os.getenv("MQTT_RECV_FILTER", config["MQTT_RECV_FILTER"])
mqtt_recv_from_fortytwo_topic = os.getenv("MQTT_RECV_FROM_42_TOPIC", config["MQTT_RECV_FROM_42_TOPIC"])
mqtt_pub_to_fortytwo_topic = os.getenv("MQTT_PUB_TO_42_TOPIC", config["MQTT_PUB_TO_42_TOPIC"])
mqtt_adcs_topic = os.getenv("MQTT_ADCS_PUB_TOPIC", config["MQTT_ADCS_PUB_TOPIC"])
mqtt_eps_topic = os.getenv("MQTT_EPS_PUB_TOPIC", config["MQTT_EPS_PUB_TOPIC"])
mqtt_adcs_actuator_topic = os.getenv("MQTT_ADCS_ACTUATOR_TOPIC", config["MQTT_ADCS_ACTUATOR_TOPIC"])
mqtt_eps_status_topic = os.getenv("MQTT_EPS_STATUS_TOPIC", config["MQTT_EPS_STATUS_TOPIC"])
mqtt_sim_message_dict_topic = os.getenv("MQTT_SIM_DATA_DICT_TOPIC", config["MQTT_SIM_DATA_DICT_TOPIC"])
mqtt_game_control_topic = os.getenv("MQTT_GAME_CONTROL_TOPIC", config["MQTT_GAME_CONTROL_TOPIC"])
mqtt_game_status_topic = os.getenv("MQTT_GAME_STATUS_TOPIC", config["MQTT_GAME_STATUS_TOPIC"])
game_state=os.getenv("GAME_START_STATE", config["GAME_START_STATE"])
payload_poiting_fov=float(os.getenv("PAYLOAD_FOV", config["PAYLOAD_FOV"]))
logging_level=os.getenv("LOG_LEVEL", config["LOG_LEVEL"])

subscscriptions = (mqtt_recv_from_fortytwo_topic, mqtt_adcs_actuator_topic, mqtt_eps_status_topic, mqtt_game_control_topic)
mqtt_qos = int(os.getenv("MQTT_QOS", config["MQTT_QOS"]), base=10)

time_data_parts = ("DynTime", "DT", "Time")
eps_msg_parts = ("EPS", "SolArr")

# print(config)
logging.basicConfig(
        level=logging.getLevelName(logging_level),
        format='%(asctime)s.%(msecs)03d %(levelname)s:\t%(message)s',
        datefmt='%Y-%m-%d %H:%M:%S')

logger = logging.getLogger("root")


def fortytwo_to_quaternion(q_fortytwo: list):
    if len(q_fortytwo) == 4:
        q_fortytwo = [float(num) for num in q_fortytwo]
        logger.debug(f"Q42 (x,y,z,s): {q_fortytwo}")
        q_fortytwo = np.array([q_fortytwo[3], q_fortytwo[0], q_fortytwo[1], q_fortytwo[2]])
        logger.debug(f"Q42 (s,x,y,z): {q_fortytwo}")
        return Quaternion(array=q_fortytwo)


def check_payload_pointing(sim_data: OrderedDict):
    payload_pointing_state = False
    qbn = fortytwo_to_quaternion(sim_data["SC[0].AC.qbn"])
    posn = np.array([float(num) for num in sim_data["SC[0].AC.PosN"]])
    veln = np.array([float(num) for num in sim_data["SC[0].AC.VelN"]])
    logger.debug(f"PosN: {posn}")
    logger.debug(f"Pos Len: {np.linalg.norm(posn)}")
    logger.debug(f"PosN Vector: {posn/np.linalg.norm(posn)}")
    logger.debug(f"VelN Unit: {veln/np.linalg.norm(veln)}")
    nadir = -(posn/np.linalg.norm(posn))
    logger.debug(f"Nadir Vector N Frame: {nadir}")
    bz = np.array([0, 0, 1])
    bz_n = qbn.rotate(bz)
    veln_norm = veln/np.linalg.norm(veln)
    # bz_n = bz_n / np.linalg.norm(bz_n)
    logger.debug(f"Body Z in N Frame: {bz_n}")
    logger.debug(f"{np.dot(bz_n, veln_norm)}")
    body_z_to_nadir_angle = np.arccos(np.dot(bz_n, nadir)) * 180 / np.pi
    logger.debug(f"Angular Differnce: {body_z_to_nadir_angle}")
    if np.abs(body_z_to_nadir_angle) < payload_poiting_fov:
        payload_pointing_state = True
    return payload_pointing_state, body_z_to_nadir_angle


# Process data going from forty to to ADCS and EPS software
async def process_outgoing_message(client: Client, message: str, forty_two_data: IncommingMessage, game_state: GameControlState):
    # Empty arrays for message lines
    adcs_message_lines = []
    eps_message_lines = []

    # Process fortytwo message
    forty_two_data.process(message)
    # await client.publish(mqtt_sim_message_dict_topic, forty_two_data.get_sim_message_json(), qos=mqtt_qos, retain=True)
    forty_two_data.sim_messeage_dict["payload_pointing_state"], forty_two_data.sim_messeage_dict["payload_offpoint_angle"] = check_payload_pointing(forty_two_data.sim_messeage_dict)
    logger.debug(f"Payload Pointing for COMMS. State: {forty_two_data.sim_messeage_dict['payload_pointing_state']}, Angle: {forty_two_data.sim_messeage_dict['payload_offpoint_angle']}")

    # Process message by line to split into ADCS and EPS components
    for line in message.splitlines():
        parts = line.split(".")
        if len(parts) > 2:
            if any(time_part in parts[2] for time_part in time_data_parts):
                eps_message_lines.append(line)
                adcs_message_lines.append(line)
                continue
            if any(eps_part in parts[2] for eps_part in eps_msg_parts):
                eps_message_lines.append(line)
                continue
            else:
                adcs_message_lines.append(line)
        else:
            eps_message_lines.append(line)
            adcs_message_lines.append(line)
    # Rejoin message lines into sendable string
    eps_message = "\n".join(eps_message_lines)
    adcs_message = "\n".join(adcs_message_lines)

    # Only forward messages if Game is in the RUNNING state
    if game_state == GameControlState.RUNNING:
        # logger.debug(f"Forty-Two Time: {forty_two_data.time_num}, SIM_TIME: {SIM_TIME_NUM}")
        # if forty_two_data.time_num > forty_two_data.time_previous:
        async with IncommingMessage.alive_lock:
            sim_msg_time = time.time()
            if IncommingMessage.adcs_alive or (sim_msg_time - IncommingMessage.poke > 10): # Poke the adcs every 10 seconds to see if it's back up (if gone down)
                sim_msg_time = time.time()
                if forty_two_data.sensor_publish_time == 0.0:
                    forty_two_data.sensor_publish_time = sim_msg_time
                    delta_time = forty_two_data.deltatime
                    logger.info(f"DELTA_TIME {delta_time}")
                wait_time = forty_two_data.deltatime - ( sim_msg_time - forty_two_data.sensor_publish_time)
                logger.debug(f"SIM_MSG_TIME {sim_msg_time}, SENSOR_PUBLISH_TIME {sim_msg_time}, WAIT_TIME {wait_time}")
                logger.debug("Next Sim Data to be published in {} seconds".format(wait_time))

                if wait_time > 0.0:
                    await asyncio.sleep(wait_time)
                forty_two_data.sensor_publish_time = time.time()
                await client.publish(mqtt_adcs_topic, adcs_message, qos=mqtt_qos, retain=True)
                await client.publish(mqtt_eps_topic, eps_message, qos=mqtt_qos, retain=True)
                await client.publish(mqtt_sim_message_dict_topic, forty_two_data.get_sim_message_json(), qos=mqtt_qos, retain=True)
                logger.info(f"Publish sensor data for timestep: {forty_two_data.time_str}")
                forty_two_data.reset()
                IncommingMessage.adcs_alive = False
                IncommingMessage.poke       = sim_msg_time
            else:
                logger.info("Adcs not up. Won't send message")
    # print(f"ADCS Lines\n{adcs_message}")
    # print(f"EPS Lines\n{eps_message}")

def index_containing_substring(the_list, substring):
    for i, s in enumerate(the_list):
        if substring in s:
              return i
    return -1

# Function to process game control MQTT messages
async def process_game_control_message(client: Client, message: MQTTMessage, current_game_state: GameControlState):
    game_state_old = current_game_state
    game_state = None
    game_state_message_payload = message.payload.decode().strip().upper()
    logger.info("Received Game State Message: {0:s}".format(game_state_message_payload))
    
    try:
        game_state_message = GameControlMessages[game_state_message_payload]
    except KeyError as e:
        logger.error(f"Got {str(e)}.")
        logger.error(f"Possible states are: START_ROUND | END_ROUND | PAUSE | RESUME")
        return current_game_state
    
    logger.info("Updated Game State is: {0:s}".format(game_state_message))
    if game_state_message in (GameControlMessages.PAUSE, GameControlMessages.END_ROUND):
        game_state=GameControlState.STOPPED
    elif game_state_message in (GameControlMessages.RESUME, GameControlMessages.START_ROUND):
        game_state=GameControlState.RUNNING
    else:
        logger.error("Unknown Game State Message {}".format(message.payload.decode()))
    if game_state_old != game_state:
        logger.info("Sending Game State Message. Game State == {}".format(game_state.name))
        await client.publish(mqtt_game_status_topic, game_state.name, qos=mqtt_qos)
    return game_state


async def send_message_to_forty_two(client, adcs_data: IncommingMessage, eps_data: IncommingMessage, game_state: GameControlState):
    message_sent = False
    if adcs_data.ready and eps_data.ready and game_state == GameControlState.RUNNING:
        logger.debug("ADCS Time: {0:s}, EPS Time: {1:s}".format(adcs_data.time_str, eps_data.time_str))
        if adcs_data.time_str == eps_data.time_str:
            msg_to_fortytwo = [line for line in adcs_data.lines]
            msg_to_fortytwo.extend([line for line in eps_data.lines[1:]])
            msg_to_fortytwo.extend(["[EOF]", "", ""])
            msg_to_fortytwo = "\n".join(msg_to_fortytwo)
            logger.debug(f"Msg To 42: {msg_to_fortytwo}")
            await client.publish(mqtt_pub_to_fortytwo_topic, msg_to_fortytwo, qos=mqtt_qos)
            logger.info(f"Timestep {adcs_data.time_str} complete")
            adcs_data.reset()
            eps_data.reset()
            msg_to_fortytwo = None
            message_sent = True
        else:
            logger.error("Times don't match, lets debug this")
            # sys.exit(1)
    return message_sent

async def run():
    adcs_actuator_data = IncommingMessage("adcs")
    eps_status_data    = IncommingMessage("eps")
    forty_two_data     = IncommingMessage("fortytwo")
    game_state         = GameControlState[os.getenv("GAME_START_STATE", config["GAME_START_STATE"]).strip().upper()]
    logger.info(f"Starting MQTT Simulation Bridge with State {game_state.name}")
    while True:
        try:
            logger.info(f"Connect to MQTT. {mqtt_hostname}:{mqtt_port} with {mqtt_user}/{mqtt_pass}")
            async with Client(
                hostname=mqtt_hostname,
                port=mqtt_port,
                username=mqtt_user,
                password=mqtt_pass, 
            ) as client:
                logger.info("Init Messages")
                await client.publish(mqtt_game_status_topic, game_state.name, qos=mqtt_qos, retain=True)
                await client.publish(mqtt_adcs_topic, "", qos=mqtt_qos, retain=True)
                await client.publish(mqtt_eps_topic, "", qos=mqtt_qos, retain=True)
                await client.publish(mqtt_sim_message_dict_topic, "", qos=mqtt_qos, retain=True)
                async with client.filtered_messages(mqtt_filter) as messages:
                    # Subscribe to all topics
                    for subscription in subscscriptions:
                        logger.info(f"Subscribe to {subscription}")
                        await client.subscribe(subscription, qos=mqtt_qos)
                    # Processes messages as they are recieved until application stops
                    async for message in messages:
                        logger.info("Recv Msg on Topic {0:s}".format(message.topic))
                        if message.topic == mqtt_game_control_topic:
                            game_state = await process_game_control_message(client, message, game_state)
                        elif message.topic in(mqtt_recv_from_fortytwo_topic):
                            await process_outgoing_message(client, message.payload.decode(), forty_two_data, game_state)
                        elif message.topic == mqtt_adcs_actuator_topic:
                            adcs_actuator_data.process(message.payload.decode())
                            async with IncommingMessage.alive_lock:
                                IncommingMessage.adcs_alive = True
                        elif message.topic == mqtt_eps_status_topic:
                            eps_status_data.process(message.payload.decode())
                        # Check that both adcs and eps messages are in the ready state
                        # Ready state means internal data structures have data
                        # Check that times match
                        msg_sent = await send_message_to_forty_two(client, adcs_actuator_data, eps_status_data, game_state)
                        logger.debug(f"Msg sent to 42? {msg_sent}")
        except MqttError as e:
            logger.error(f"MQTT Error: {e}")
            await asyncio.sleep(5)

if __name__ == "__main__":

    # Run control function
    asyncio.run(run())

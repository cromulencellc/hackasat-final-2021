#!/usr/bin/env python3

# Cromulence
# Author: meenmachine

import aioconsole
import argparse
import asyncio
import base64
import json
import logging
import os
import secrets
import struct
import sys
from threading import Lock
import uuid

from datetime import datetime
from hashlib import sha256
from math import floor

MINUTES_PER_ROUND  = 10
PINGS_PER_MINUTE   = 60 # 1 ping per second
HOURS_PER_GAME     = 24
MAX_NUM_ROUNDS     = 0xFF

MINUTES_PER_HOUR   = 60
SECONDS_PER_MINUTE = 60
MINUTES_PER_GAME   = HOURS_PER_GAME * MINUTES_PER_HOUR
ROUNDS_PER_GAME    = MINUTES_PER_GAME // MINUTES_PER_ROUND

CONFIGS_PATH  = "/configs"

USER_SEG_ADDR = '0.0.0.0'
USER_SEG_PORT = 3099

# CONFIGS
LOG_LEVEL   = logging.DEBUG
LOG_FOLDER  = "logs"
LOG_TO_FILE = True

logger = logging.getLogger(__name__)

# Sys write flush
def p_f(s):
    sys.stdout.write(s)
    sys.stdout.flush()

def p_last_pings(controller):
    os.system("clear")
    sys.stdout.write("### Ping Controller ###\n")
    
    sys.stdout.write(f"1. Start Round {controller.cur_round}\n")
    sys.stdout.write(f"2. End Round {controller.cur_round - 1}\n\n")
    
    sys.stdout.write(f"CURRENT ROUND: {controller.cur_round} : {controller.cur_seq}\n\n")
    sys.stdout.write(f"Last 5 pings:\n")
    for ping_num in range(controller.max_ping_saved):
        with controller.lock:
            sys.stdout.write(f"Ping{ping_num}:\n\tSync: {controller.last_pings[ping_num]['sync']}\n")
            sys.stdout.write(f"\tRoundNum: {controller.last_pings[ping_num]['round_num']}\n")
            sys.stdout.write(f"\tSeqNum: {controller.last_pings[ping_num]['seq_num']}\n")
            sys.stdout.write(f"\tPing: {controller.last_pings[ping_num]['ping']}\n")
    sys.stdout.write("\n")
    sys.stdout.flush()
    
async def prompt(cur_round):
    p_f(f"1. Start Round {cur_round}\n")
    p_f(f"2. End Round {cur_round - 1}\n")

class ControllerServerProtocol:
    def __init__(self):
        self.tmp            = 0
        self.lock           = Lock()
        self.last_pings     = [{"sync": b'', 'round_num': b'', "seq_num": b'', "data": b'', "ping": b''},
                               {"sync": b'', 'round_num': b'', "seq_num": b'', "data": b'', "ping": b''},
                               {"sync": b'', 'round_num': b'', "seq_num": b'', "data": b'', "ping": b''},
                               {"sync": b'', 'round_num': b'', "seq_num": b'', "data": b'', "ping": b''},
                               {"sync": b'', 'round_num': b'', "seq_num": b'', "data": b'', "ping": b''}]
        self.num_pings      = 0
        self.max_ping_saved = 5
        self.cur_round      = 1
        self.cur_seq        = 1
        
        super().__init__()
    
    def connection_made(self, transport):
        self.transport = transport
        logger.debug("Made connection to user_segment server")
        # self.transport.sendto(b"msg")
    
    def datagram_received(self, data, addr):
        dest    = data[:1]
        payload = data[1:]  #strip off destination payload to send to apps
        
        if len(payload) < 8:
            logger.error(f"Packet too short")
        
        sync_word, payload_round_num, payload_seq_num = b'', 1, 1
        with self.lock:
            self.last_pings[self.num_pings % self.max_ping_saved]["data"] = data
            try:
                sync_word, payload_round_num, payload_seq_num = struct.unpack("<5sBH", payload[:8])
                payload = payload[5:]
                self.last_pings[self.num_pings % self.max_ping_saved]["round_num"] = payload_round_num
                self.last_pings[self.num_pings % self.max_ping_saved]["seq_num"]   = payload_seq_num
                self.last_pings[self.num_pings % self.max_ping_saved]["sync"]      = sync_word
                self.last_pings[self.num_pings % self.max_ping_saved]["ping"]      = struct.unpack("Q", payload)[0]
                
                self.num_pings += 1
                self.cur_round = payload_round_num
                self.cur_seq   = payload_seq_num
            except struct.error as e:
                logger.warning(f"Couldn't parse packet: {data}. Got: {str(e)}")
                return
        
        p_last_pings(self)
        logger.debug(f"Round {payload_round_num}:{payload_seq_num} | Recieved: {data}")
    
    def error_received(self, exc):
        logger.error(exc)
        
    def connection_lost(self, exc):
        logger.warning("Connection lost")

async def generate_pings(round_num, round_ping_path, rounds):
    ping_seed = secrets.token_bytes(128)
    ping_b64  = base64.b64encode(ping_seed)
    
    pings = {"seed": {
                     "b64": str(ping_b64),
                     "num": int.from_bytes(ping_seed, byteorder="little")        
                     },
             "pings": {}
            }
    
    num_pings_in_round = rounds[round_num]
    for cur_ping in range(1, num_pings_in_round + 1):
        ping = ping_b64 + b"_r" + str(cur_ping).encode() + b"_p" + str(cur_ping).encode()
        
        ping_sha = sha256(ping).digest()[:5]
        
        ping_final = struct.pack("B", round_num) + struct.pack("H", cur_ping) + ping_sha
        ping_final = struct.unpack("Q", ping_final)[0]
                
        pings["pings"][cur_ping] = ping_final
    with open(round_ping_path, "w") as ping_file:
        json.dump(pings, ping_file)
    
async def check_pings_dir(ping_path):
    # <ROUND_NUM> : <ROUND IN HOURS>
    rounds = {}
    
    if not os.path.exists(ping_path):
        os.mkdir(ping_path)
    if not os.path.isdir(ping_path):
        logger.fatal(f"{ping_path} is not a folder. Delete and rerun.")
        logger.fatal("Bailing out...")
        exit(-1)
    
    for round_num in range(1, ROUNDS_PER_GAME+1):
        rounds[round_num] = MINUTES_PER_ROUND * PINGS_PER_MINUTE
    
    logger.info("VERIFYING ping files. This could take a few seconds...")
    
    for round_num in rounds:
        round_ping_path = f"{ping_path}/round_{round_num}.json"
        if not os.path.exists(round_ping_path):
            await generate_pings(round_num, round_ping_path, rounds)
        elif os.path.isfile(round_ping_path):
            round_ping = {}
            try:
                with open(round_ping_path, "r") as ping_file:
                    round_ping = json.load(ping_file)

                if not "seed" in round_ping:
                    raise json.JSONDecodeError("No 'seed' key in json. Delete and rerun")
                elif not "pings" in round_ping:
                    raise json.JSONDecodeError("No 'pings' key in json. Delete and rerun")
                elif len(round_ping['pings']) != rounds[round_num]:
                    raise json.JSONDecodeError(f"Number of pings in round {round_num} not equal to defined {rounds[round_num]}")
            except json.JSONDecodeError as e:
                logger.fatal(f"{round_ping_path} is not formatted correctly. Got: {str(e)}")
                logger.fatal("Bailing out")
                exit(-1)
        else:
            logger.fatal(f"{round_ping_path} is not a file")
            logger.fatal("Bailing out...")
            exit(-1)
async def main():
    await check_pings_dir(f"{CONFIGS_PATH}/round_pings")
    
    loop = asyncio.get_running_loop()
        
    transport, protocol = await loop.create_datagram_endpoint(
        lambda: ControllerServerProtocol(),
        local_addr=(USER_SEG_ADDR, USER_SEG_PORT)
    )
    
    os.system("clear")
    p_f("### Ping Controller ###\n")
    
    cur_round = 1
    while True:
        await prompt(cur_round)
        line = await aioconsole.ainput(" > ")
        try:
            line = int(line)
        except ValueError:
            p_f("Invalid input\n")
            continue

def handle_logs():
    global LOG_FOLDER
    global LOG_TO_FILE
    global LOG_LEVEL
    loggerFormat = '%(asctime)s %(levelname)s %(message)s'

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

    gen_log_name = lambda : f"controller_{str(uuid.uuid4())[:8]}_{datetime.now().strftime('%m-%d_%H-%M')}"

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
    
    logger = logging.getLogger(__name__)
    
if __name__ == "__main__":
    handle_logs()
    asyncio.run(main())
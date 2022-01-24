#!/usr/bin/python3

import socket, os, time
import threading
import logging
import logging.handlers

logging.basicConfig(format='%(asctime)s TLM_SERVER: %(message)s')
logger = logging.getLogger('TLM_SERVER')
logger.setLevel(logging.DEBUG)

# handler = logging.handlers.SysLogHandler(address = '/dev/log')
# logger.addHandler(handler)

from ballcosmos import *
from ballcosmos import telemetry


def conn_handler(conn, addr):

    while True:

        data = conn.recv(2048)

        if not data:
            break

        logger.info(f"Server got a raw tlm object from addr {addr[0]} of len {len(data)}")
        
        try:
            data = data.decode()
        except:
            logger.critical("can't decode data from {addr[0]}...closing connection")
            break

        logger.debug(f"Looking up: {data} for {addr[0]}")

        try:
            result = tlm_formatted(data)
        except Exception as e:
            result = f"** invalid request ** Exception: {e}\n"

        logger.debug(result)
        result = result.encode() + b"\n"
        conn.send(result)

        # Make sure teams can't spam it too quickly
        time.sleep(.25)

    conn.send(b"Closing connection")
    conn.close()


def init():


    HOST = "0.0.0.0"
    if 'SERVER_PORT' in os.environ:
        RAW_PORT = int(os.environ['SERVER_PORT'])
    else:
        RAW_PORT = 1337

    logger.info(f"Starting raw tlm server at address {HOST} port {RAW_PORT}")

    raw_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    raw_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    raw_socket.bind((HOST, RAW_PORT))
    raw_socket.listen(1)

    while True:

        conn, addr = raw_socket.accept()

        t = threading.Thread(target=conn_handler, args=(conn,addr))
        t.start()

init()
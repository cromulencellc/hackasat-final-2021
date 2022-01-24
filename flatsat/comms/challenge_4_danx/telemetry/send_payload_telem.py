#!/usr/bin/env python3

import socket, struct, os, sys
import paho.mqtt.publish as publish

from time import sleep
from sys import getsizeof

#INBOUND_IP = '172.17.0.1'
INBOUND_IP = '127.0.0.1'
TEAM_PORT = [4581,
             4582,
             4583,
             4584,
             4585,
             4586,
             4587,
             4588]

mqtt_user = os.getenv('MQTT_USER', "comm")
mqtt_pass = os.getenv("MQTT_PASS", "2B5458962584FE417B2803CF")
MQTT_HOSTNAME = os.getenv("MQTT_IP", "192.168.101.101")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"), base=10)

STATUS_TOPIC = "COMM/PAYLOAD/TELEMETRY"

teamsock = [0,0,0,0,0,0,0,0]

data = [0,0,0,0,0,0,0,0]
connected = [False for x in range(1,8+1)]

def connect_to_challenge(team, reconnect=False):
    if reconnect:
        teamsock[team] = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        teamsock[team].settimeout(.1)
    try:
        teamsock[team].connect((INBOUND_IP, TEAM_PORT[team]))
        try:
            buf = teamsock[team].recv(8)
            if buf == b'COMREADY':
                print(f"Team[{team+1}]: Connection establihsed: {buf}")
                connected[team] = True
                return True;
        except:
            pass

    except ConnectionRefusedError:
        pass

    except socket.timeout as e:
        pass

    return False

for team in range(0,len(TEAM_PORT)):
    teamsock[team] = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    teamsock[team].settimeout(.1)
    while True:
        if connect_to_challenge(team):
            break

while(True):
    print("----")
    for team in range(0,len(TEAM_PORT)):
        try:
            if connected[team]:
                buf = teamsock[team].recv(8)
                if len(buf) == 8:
                    f = struct.unpack("<Q", buf)[0]
                    data[team] = f
                    print(f"Team[{team+1}]: data: ({buf})")
                if len(buf) == 0:
                    if connected[team]:
                        print(f'Team[{team+1}]: Disconnected from Telemtry')
                        connected[team] = False
                        connect_to_challenge(team, reconnect=True)

            else:
                connect_to_challenge(team, reconnect=True)

        except socket.timeout:
            pass

    payload = struct.pack("<8Q", *data)

    try:
        publish.single(STATUS_TOPIC, hostname=MQTT_HOSTNAME, port=MQTT_PORT, payload=payload, keepalive=1, qos=2, auth={'username': mqtt_user, 'password': mqtt_pass})
    except OSError as e:
        print(f"MQTT send error: {str(e)}")
    sleep(.5)


#!/usr/bin/python3

import argparse
import asyncio
from gmqtt import Client as MQTTClient
from gmqtt.mqtt.constants import MQTTv311
STOP = asyncio.Event()

def on_connect(client, flags, rc, properties):
    print('Connected')

async def main(host, port, topic, qos, user, password, payload):

    client = MQTTClient('clientid')
    client.set_auth_credentials(user, password)
    await client.connect(host, port, keepalive=5, version=MQTTv311)
    client.publish(topic, payload, qos)
    await client.disconnect()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("-h", "--host", help="Hostname")
    parser.add_argument("-p", "--port", help="Port", type=int)
    parser.add_argument("-t", "--topic", help="Topic")
    parser.add_argument("-q", "--qos", help="QOS", type=int)
    parser.add_argument("-u", "--user", help="User")
    parser.add_argument("-P", "--password", help="Password")
    parser.add_argument("-m", "--message", help="Payload message")
    args=parser.parse_args()
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main(args.host, args.port, args.topic, args.qos, args.user, args.password, args.message))
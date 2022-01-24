#!/usr/bin/python3
import os

import paho.mqtt.client as mqtt
client = mqtt.Client()
client.username_pw_set('comm', 'comm')

MQTT_IP = os.environ['MQTT_IP']
STATUS_TOPIC = "COMM/PAYLOAD/SLA"
client.connect(MQTT_IP, 1883)

def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))                                                                                                                            
                          
client.on_message = on_message                                                  
client.subscribe(STATUS_TOPIC)                                       
        
client.loop_forever()
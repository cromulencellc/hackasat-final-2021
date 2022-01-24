#!/usr/bin/env python3

import socket
import time
import random 

SCORE_IP = "localhost"
SCORE_PORT = 4001

print(f"UDP target IP: {SCORE_IP}")
print(f"UDP target port: {SCORE_PORT}")

#for idx in range(0,,1):

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
while True:
    randomlist = random.sample(range(1000,9999), 8)
    print(randomlist)
    msg2=""
    for idx in range(0,8,1):
        #print(f"team: {idx}: {randomlist[idx]}")
        msg2=msg2+str(randomlist[idx])
    print(f"ScoreSvr: {msg2}")
    
    sock.sendto(msg2.encode(), (SCORE_IP, SCORE_PORT))
    time.sleep(10)




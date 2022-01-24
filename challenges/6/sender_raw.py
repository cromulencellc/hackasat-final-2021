#!/usr/bin/python3

import socket, time

HOST = '0.0.0.0'
PORT = 1337

def send_raw(data):
    #print(f"SNDR: Connecting to HOST:{HOST} PORT:{PORT}")
    s = socket.create_connection((HOST, PORT))

    #print(f"SNDR: {data}")
    s.send(bytes(data.encode()))    

    #print(f"SNDR: Sent data of size {len(data)}")
    response = s.recv(1024)
    print(f"{data} = {response.decode()}")

    s.close()

def init():
    tlmentries = ["CF HK_TLM_PKT CCDS_STREAMID",
                  "ADCS HK_TLM_PKT CMD_VALID_COUNT",
                  "ADCS HK_TLM_PKT CMD_ERROR_COUNT",
                  "ADCS HK_TLM_PKT LAST_TBL_ACTION",
                  "ADCS HK_TLM_PKT LAST_TBL_STATUS",
                  "ADCS HK_TLM_PKT CTRL_EXEC_COUNT",
                  "ADCS HK_TLM_PKT CTRL_MODE",
                  "ADCS HK_TLM_PKT CTRL_EXEC_DELAY",
                  "ADCS HK_TLM_PKT HW_EXEC_COUNT",
                  "SLA_TLM HK_TLM_PKT ATTRIBUTION_KEY",
                  "COMM HK_TLM_PKT COMM_STATUS_STRING"]

    for tlm in tlmentries:
        send_raw(tlm)
        #time.sleep(1)
        #print("SNDR -----")

init()
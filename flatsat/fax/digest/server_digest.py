#!/usr/bin/python3

import pickle, socket, os
from digest import Digest

def init():
    HOST = "0.0.0.0"
    print(os.environ)
    DIGEST_PORT = int(os.environ['SERVER_PORT'])

    print(f"svrdig: Starting digest server at address {HOST} port {DIGEST_PORT}")

    digest_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    digest_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    digest_socket.bind((HOST, DIGEST_PORT))
    digest_socket.listen(1)

    while True:
        conn, addr = digest_socket.accept()

        data = conn.recv(2048)

        print(f"svrdig: Server got a digest object from addr {addr} of len {len(data)}")

        try:
            digest_obj = pickle.loads(data)
            print(f"svrdig: Digest object created: {digest_obj}")

            tlmoutput = digest_obj.run()

            print(f"svrdig: Output from Digest object: {tlmoutput}")

            conn.send(tlmoutput.encode())  #bytes(tlmoutput))

            conn.close()
        except Exception as e:
            conn.send(f"svrdig: Digest failed: {str(e)} Closing connection!".encode())
            conn.close()

init()
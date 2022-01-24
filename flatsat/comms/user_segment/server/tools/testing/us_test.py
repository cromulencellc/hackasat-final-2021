#!/usr/bin/env python3

# import asyncio
import struct
import codecs
import argparse
import secrets
import socket

from Crypto.PublicKey import RSA
from Crypto.Signature import pkcs1_15
from Crypto.Hash import SHA256

CONFIG_PATH = "../../configs"

USS_IP   = "10.0.0.101"
USS_PORT = 31337

def main():
    parser = argparse.ArgumentParser(description='Test user segment')

    parser.add_argument('-t',
                        '--team',
                        metavar="1",
                        type=int,
                        help="Team number",
                        default=1)

    args = parser.parse_args()
    
    team_num = args.team

    # Get a reference to the event loop as we plan to use
    # low-level APIs.

    pl = b""
    with open("payload.bin", "rb") as f:
        pl = f.read()

    with open(f'{CONFIG_PATH}/team_private_keys/team_{team_num}_rsa_priv.pem', 'rb') as f:
        private_key = RSA.import_key(f.read())

    sync_msg  = b"AAAA"
    team_id   = 1
    attr_key  = 3367096896938054122 # 64bit number
    message   = pl
    msg_len   = len(message)

    packet = struct.pack(f"<4sQQH{msg_len}s", sync_msg, team_id, attr_key, msg_len, message)
    print(f"Sending: {message} | packet: {packet}")

    digest = SHA256.new(packet)

    signature = pkcs1_15.new(private_key).sign(digest)

    with open(f'{CONFIG_PATH}/team_public_keys/team_{team_num}_rsa_pub.pem', 'rb') as f:
        public_key = RSA.import_key(f.read())
    
    digest_verify = pkcs1_15.new(public_key).verify(digest, signature)

    print(digest_verify)
    print(signature)
    print(len(signature))

    packet = struct.pack(f"{len(signature)}s{len(packet)}s", signature, packet)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((USS_IP, USS_PORT))
    s.send(packet)

if __name__ == "__main__":
    main()

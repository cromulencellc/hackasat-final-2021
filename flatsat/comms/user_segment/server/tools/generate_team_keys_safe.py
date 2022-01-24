#!/usr/bin/env python3

import secrets
import time
import sys
from Crypto.PublicKey import RSA
from os.path import exists
from os import remove

PATH_TO_CONFIG = "../configs"

NUM_TEAMS      = 8
WAIT_BTWN_KEYS = 5
RSA_BITS       = 2048
RSA_BYTES      = int(RSA_BITS / 8)

def gen_keys_good(teams=NUM_TEAMS,conf_path=PATH_TO_CONFIG,wait_time=WAIT_BTWN_KEYS):
    sys.stdout.write(f"Generating {teams} keys. This could take up to {wait_time*teams} seconds...\n")
    sys.stdout.flush()
    for team_num in range(1,teams+1):
        # Sleep an unpredictable period of time between keys so keys aren't predictable
        rand_sleep_time = int(secrets.token_hex(2), 16) % wait_time

        sys.stdout.write(f"Generating team #{team_num} keys\n")
        sys.stdout.flush()
        time.sleep(rand_sleep_time)

        keyPair = RSA.generate(bits=RSA_BITS)

        privKey = keyPair.export_key()
        pubKey  = keyPair.publickey().export_key()

        with open(conf_path + "/team_private_keys/" + f"team_{team_num}_rsa_priv.pem", 'wb') as f:
            f.write(privKey)

        with open(conf_path + "/team_public_keys/" + f"team_{team_num}_rsa_pub.pem", 'wb') as f:
            f.write(pubKey)

        keyPair = None

    bad_indicator_path = conf_path + "/team_public_keys/" + "BAD"
    if(exists(bad_indicator_path)):
        remove(bad_indicator_path)

    sys.stdout.write(f"Done generating keys\n")
    sys.stdout.flush()

if __name__ == "__main__":
    gen_keys_good()
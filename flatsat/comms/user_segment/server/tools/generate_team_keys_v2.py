#!/usr/bin/env python3

import os
import subprocess
import yaml
import secrets
import struct
import hashlib
from math import ceil
import time

from sys import maxsize as UINT64_T_MAX_SIZE # 9_223_372_036_854_775_807

### Toggle Variables ###
# If TEST is true, when you run the script as main it will generate TEST_NUM_ITERATIONS random keys
TEST                = False
TEST_NUM_ITERATIONS = 1000

### Game Configuration Variables ###
NUM_TEAMS               = 8
RSA_BITS                = 1028 + 172 + 172
RSA_BYTES               = ceil(RSA_BITS / 8)

SECRET_RANDOM_NUMS_FILE = "team_secret_random_nums.yaml"
CONFIG_PATH             = f"{os.getcwd()}/../configs"

BINARY                  = "gen"
KEYGEN_PATH             = f"{os.getcwd()}/key_gen"

def gen_team_random_nums(num_teams=NUM_TEAMS, conf_path=CONFIG_PATH):
    if not(os.path.exists(conf_path) and os.path.isdir(conf_path)):
        print(f"(KEYGEN) Error: Config path '{os.getcwd()} / {conf_path}' does not exist or is not a directory.")
        return None
    
    rand_nums_dict = None
    if not(os.path.exists(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}")):
        rand_nums_dict = {}
        with open(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}", 'w') as random_nums_file:
            for team_num in range(1, num_teams+1):
                rand_num = struct.unpack("Q", secrets.token_bytes(8))[0]
                
                while((rand_num % 787 == 0) or (rand_num % 1579 == 0) or (rand_num % 3163 == 0) or (rand_num >= UINT64_T_MAX_SIZE)):
                    if rand_num < UINT64_T_MAX_SIZE:
                        print(f"(KEYGEN) (Ignore) Stats: Rand num not relatively prime to magic M {rand_num}")
                    rand_num = struct.unpack("Q", secrets.token_bytes(8))[0]
                
                rand_nums_dict[team_num] = rand_num
            
            yaml.safe_dump(rand_nums_dict, random_nums_file)
            print(f"(KEYGEN) Info: Generated rand_nums_dict: {rand_nums_dict}")
    elif os.path.isfile(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}"):
        with open(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}", 'r') as random_nums_file:
            rand_nums_dict = yaml.safe_load(random_nums_file)
        
        if (not rand_nums_dict) or (len(rand_nums_dict) != num_teams):
            print(f"(KEYGEN) Error: {conf_path}/{SECRET_RANDOM_NUMS_FILE} not formatted correctly.")
            return None
    else:
        print(f"(KEYGEN) Error: {conf_path}/{SECRET_RANDOM_NUMS_FILE} is not a file.")
        return None
    
    return rand_nums_dict

def gen_keys_client(num_teams=NUM_TEAMS, conf_path=CONFIG_PATH, kg_path=KEYGEN_PATH, gen_new_secrets=False):
    if not (os.path.exists(f"{kg_path}/{BINARY}") and os.path.isfile(f"{kg_path}/{BINARY}")):
        print(f"(KEYGEN) Error: Binary '{os.getcwd()} / {kg_path}/{BINARY}' does not exist.")
        return False

    if gen_new_secrets:
        os.remove(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}")
    
    rand_nums = gen_team_random_nums(num_teams, conf_path)

    if(rand_nums is None):
        print(f"(KEYGEN) Error: {conf_path}/{SECRET_RANDOM_NUMS_FILE} is not formatted correctly.")
        return False

    os.chdir(kg_path)

    dups = True
    while(dups):
        dups = False
        keys_hash_map = {}
        for team_num in range(1, num_teams+1):
            # Change this into a docker run
            gen_env = os.environ.copy()
            gen_env["TEAM_RANDOM_NUM"] = str(rand_nums[team_num])
            gen_env["TEAM_NUM"] = str(team_num)
            gen_env["LD_LIBRARY_PATH"] = "/opt/ssl/:./openssl/build"
            # # # # # # # # # # # # # # # #

            gen = subprocess.Popen([f"./{BINARY}", f"{conf_path}/team_public_keys", f"{conf_path}/team_private_keys"], env=gen_env, stdout=subprocess.PIPE)
            # print(gen.stdout.read())
            gen.communicate()

            if gen.returncode != 0:
                print("(KEYGEN) FATAL: gen binary return a non-zero value. Recheck binary source.")
                return False

            team_priv_key_path = f"{conf_path}/team_private_keys/team_{team_num}_rsa_priv.pem"

            if(not (os.path.exists(team_priv_key_path) and os.path.isfile(team_priv_key_path))):
                print(f"(KEYGEN) Error: Binary did not generate private key for team {team_num}.")
                return False
            
            with open(team_priv_key_path, 'rb') as team_priv_key_f:
                cur_team_key_hash = hashlib.md5(team_priv_key_f.read()).hexdigest()
            
            if cur_team_key_hash in keys_hash_map:
                print("(KEYGEN) WARNING: More than one private key is the same. Regenerating team_secret_random_nums.yml")
                
                # os.chdir("..")
                if os.path.exists(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}") and os.path.isfile(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}"):
                    os.remove(f"{conf_path}/{SECRET_RANDOM_NUMS_FILE}")
                else:
                    print(f"(KEYGEN) Error: '{os.getcwd()} / {conf_path}/{SECRET_RANDOM_NUMS_FILE}' either doesn't exist or is not a file.")
                    return False

                gen_team_random_nums(num_teams, conf_path)

                os.chdir(kg_path)

                dups = True

                break
            else:
                keys_hash_map[cur_team_key_hash] = True
    
    # os.chdir("..")

    return True

if __name__ == "__main__":
    if TEST:
        for test_iteration in range(TEST_NUM_ITERATIONS):
            gen_keys_client(NUM_TEAMS, CONFIG_PATH)
            if os.path.exists(f"{CONFIG_PATH}/{SECRET_RANDOM_NUMS_FILE}") and os.path.isfile(f"{CONFIG_PATH}/{SECRET_RANDOM_NUMS_FILE}"):
                os.remove(f"{CONFIG_PATH}/{SECRET_RANDOM_NUMS_FILE}")
    else:
        gen_keys_client(NUM_TEAMS, CONFIG_PATH)
        # gen_keys_client(NUM_TEAMS, CONFIG_PATH, gen_new_secrets=True)
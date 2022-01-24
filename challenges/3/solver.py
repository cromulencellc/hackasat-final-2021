#!/usr/bin/env python3

from Crypto.PublicKey import RSA

import argparse
import subprocess
import yaml
import sys

from tools.rsatool import RSATool

REMOTE_TAG = "crypto:cado"
NUM_TEAMS = 8

PEM_TEMPLATE = b'-----BEGIN RSA PRIVATE KEY-----\n%s-----END RSA PRIVATE KEY-----\n'
DEFAULT_EXP = 2**16+1

RSA_SMALL_BITS = 172+172
RSA_BITS       = RSA_SMALL_BITS+1028

# Insert Team r values here (r being the third prime)
TEAM_R_VALUES = [
    0x1,
    0x2,
    0x3,
    0x4,
    0x5,
    0x6,
    0x7,
    0x8
]

def verify_keys(cracked_keys):
    passed = True
    for team_num in cracked_keys:
        if(TEAM_R_VALUES[team_num-1] * cracked_keys[team_num][0] * cracked_keys[team_num][1]):
            print(f"Team {team_num} passed checked")
        else:
            print(f"!BAD! Team {team_num} DID NOT pass check")
            passed = False
    
    return passed

def solve(team_to_crack=0):
    passed = True
    
    small_keys   = {}
    cracked_keys = {}
    if team_to_crack == 0:
        for team_num in range(1, NUM_TEAMS+1):
            pub_key = ""
            with open(f"pub_keys/team_{team_num}_rsa_pub.pem", 'rb') as f:
                pub_key = f.read()
                pub_key = RSA.import_key(pub_key)
            
            team_n = pub_key.n
            
            crack_me = team_n // TEAM_R_VALUES[team_num-1]
            small_keys[team_num] = crack_me
            print(f"Team {team_num} {RSA_SMALL_BITS}bit n is: {crack_me}")
            sys.stdout.flush()
    else:
        pub_key = ""

        with open(f"pub_keys/team_{team_to_crack}_rsa_pub.pem", 'rb') as f:
            pub_key = f.read()
            pub_key = RSA.import_key(pub_key)
        
        team_n = pub_key.n
        
        crack_me = team_n // TEAM_R_VALUES[team_to_crack-1]
        small_keys[team_to_crack] = crack_me

        print(f"Team {team_to_crack} 256bit n is: {crack_me}")

    # Iterate through keys and crack them
    for team_num in small_keys:
        print(f"Cracking team {team_num}. This will take a few minutes")

        crack = subprocess.Popen(["docker", "run", "-it", "--rm", REMOTE_TAG, str(small_keys[team_num]), "-t", "all"],
                                 stdout=subprocess.PIPE)
        
        out = crack.communicate()
        out = out[0]

        # The last line of Cado's output will be the two primes
        nums = out.split(b"\n")[-2]
        
        num_0 = nums.split(b" ")[0].decode()
        num_1 = nums.split(b" ")[1].decode()
        
        try:
            num0 = int(num_0)
            num1 = int(num_1)
            
            cracked_keys[team_num] = [num0, num1]
            
            print(f"Team {team_num} small primes are: {num0} and {num1}")
        except ValueError as e:
            print(f"Could not parse primes we got for team {team_num} with partial key: {small_keys[team_num]}.")
            print(f"Received from cado: {num_0} and {num_1}")
            print(f"Got ValueError: {str(e)}")
            
            print(f"Skipping team {team_num}. Trying next team...")
            passed = False
            
    passed = passed and verify_keys(cracked_keys)
    
    private_nums = {}
    
    for team_num in cracked_keys:
        factors = [cracked_keys[team_num][0], cracked_keys[team_num][1], TEAM_R_VALUES[team_num-1]]
        args = factors + [DEFAULT_EXP]
        
        priv_key = crypto.crypto.rsa_private_key(*args, totient="Carmichael")
        n = priv_key[0]
        d = priv_key[1]
        
        key = RSATool(n=n, d=d, e=DEFAULT_EXP)
        # key.dump(False)
        
        data = key.to_pem()
        
        private_nums[team_num] = {"n": n, "d": d, "factors": factors, "key": data}
        
        with open(f"keys/team_{team_num}_key.pem", "wb") as f:
            f.write(data)

    with open("team_keys_cracked.yaml", "w") as f:
        yaml.safe_dump(private_nums, f)

    if passed:
        print("Successfully cracked team(s) keys...")
    else:
        print("Failed to crack team(s) keys...")

def quick_solve(team_num):
    private_nums = {}
    with open("team_keys_cracked.yaml", "rb") as f:
        private_nums = yaml.safe_load(f.read())
    
    n = private_nums[team_num]["n"]
    d = private_nums[team_num]["d"]
    key = RSATool(n=n, d=d, e=DEFAULT_EXP)
    key.dump(True)
    data = key.to_pem()
    
    with open(f"keys/team_{team_num}_key.pem", "wb") as f:
        f.write(data)
        
    print(f"Finished dumping team {team_num}")

def quick_solve_all():
    private_nums = {}
    with open("team_keys_cracked.yaml", "rb") as f:
        private_nums = yaml.safe_load(f.read())
    
    for team_num in range(1, NUM_TEAMS+1):
        n = private_nums[team_num]["n"]
        d = private_nums[team_num]["d"]
        key = RSATool(n=n, d=d, e=DEFAULT_EXP)
        key.dump(True)
        data = key.to_pem()
        
        with open(f"keys/team_{team_num}_key.pem", "wb") as f:
            f.write(data)
        
    print(f"Finished dumping team {team_num}")

if __name__ == "__main__":
    args = argparse.ArgumentParser()
    args.add_argument("--team",
                      "-t",
                      help="Team pub key to crack",
                      type=int,
                      default=0)
    
    args = args.parse_args()
    
    solve(args.team)
    # quick_solve_all()
    # quick_solve(args.team)
#!/usr/bin/env python3

import os
import secrets
import sys

ADCS_DEFS_PATH = "osk_adcs_defs"
FLAG_FILENAME  = "flag.txt"

FLAG_LEN = 8    # in bytes

def check_file_exists(path):
    if os.path.exists(path) and not os.path.isdir(path):
        return True

    return False

def check_flag_format(path):
    flag = b''
    with open(path, "rb") as flag_f:
        flag = flag_f.read()
        
    if len(flag) != FLAG_LEN:
        return False
    
    sys.stdout.write(f"VERIFIED: Team {os.getenv(key='TEAM_NUMBER')} flag: {flag} | length: {len(flag)}\n")
    sys.stdout.flush()
    
    return True

def gen_flag(path):
    flag = secrets.token_bytes(FLAG_LEN)

    with open(f"{ADCS_DEFS_PATH}/{FLAG_FILENAME}", 'wb') as flag_f:
        while len(flag) != FLAG_LEN:
            flag = secrets.token_bytes(FLAG_LEN)
        
        sys.stdout.write(f"WRITING Team {os.getenv(key='TEAM_NUMBER')} flag: {flag} | length: {len(flag)} | TO {path}\n")
        sys.stdout.flush()
        
        bytes_written = flag_f.write(flag)
        
        if bytes_written != FLAG_LEN:
            sys.stdout.write(f"ERROR: Could not write flag to {path}\n")
            return False
        else:
            return True

def main():
    flag_path = f"{ADCS_DEFS_PATH}/{FLAG_FILENAME}"
    
    if not check_file_exists(flag_path):
        flag_gen = gen_flag(flag_path)
        if not flag_gen:
            sys.stdout.write(f"PLEASE CHECK THE path {ADCS_DEFS_PATH}/{FLAG_FILENAME}.")
            sys.stdout.flush()

            exit(-1)
    else:
        flag_check = check_flag_format(flag_path)

        if not flag_check:
            flag_gen = gen_flag(flag_path)
            if not flag_gen:
                sys.stdout.write(f"PLEASE CHECK THE path {ADCS_DEFS_PATH}/{FLAG_FILENAME}.")
                sys.stdout.flush()

                exit(-1)

    sys.stdout.flush()

if __name__ == "__main__":
    main()
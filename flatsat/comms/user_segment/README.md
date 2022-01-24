# User Segment and Challenge 3

Cromulence
Author: meenmachine

This contains all the user_segment components:
 - Client used by teams to communicate with the server
 - Server
 - Crypto key generation
 - Ping generations

The user segment server was a shared segment that teams could send commands to their satellite through.
It allowed for communications to the comms app on the C&DH as well as the DANX service on the comm radio board.

## Installing dependencies

Both client and server require [docker](https://docs.docker.com/engine/install/ubuntu/). If running user segment in containers, install docker.

## Pulling server and client images

```shell
cd user_segment
./build.sh
```

This will pull all the docker images from the registry for you that are needed for the user segment server and client

## Generate Team's Client Tarballs

These tarballs will have a copy of the client binary, a copy of the team's private and primary keys 
and a copy of the team's attribution keys.

First build the docker container used to compile the binaries:

```shell
cd user_segment/
./build.sh
```

Then run the script used to generate the client binaries for all the teams

```shell
cd client
./gen_team_clients.sh
```

Team files will be found in user_segment/client/build/team_files

## Running

### Server:

- Running in Docker

First build the docker container the server runs in:

```shell
cd user_segment/
./build.sh
```

```shell
cd user_segment/server
./start.sh -s
```

`start.sh` has usage help. Appended args will be added to the default args defined in `user_segment/server/docker-compose.yaml`

- Running Locally and Testing with MQTT
  - Start up the mosquitto server. (See digitaltwin repo for mosquitto instructions)
  - Activate the python virtual enviroment using `source activate` in the `server` directory and deactivate using `deactivate`.

  - Run the `server.py` script
    - Use args `--dispatch` for radio usage or `--mqtt` to post directly onto MQTT

  Public and private keys will be AUTOMATICALLY GENERATED for you when `server.py` runs. Do not try 
  to generate keys without using `server.py` or `server/tools/generate_team_keys_v2.py`

  Keys are stored in the `configs` folder. If no keys exist, keys will be made for you. If keys
  exist they will not be overwritten. If you would like to generate new keys with `server.py`, make sure all the keys
  are deleted in the `configs/team_public_keys/` folder.

### Client:
In `user/client` folder
- Run `client` binary with proper arguments. Example:
```shell
./client --id  1 \ 
         --key 1 \ 
         --data 1A \ 
         --key-file ../server/configs/team_private_keys/team_1_rsa_priv.pem
```
- Client binary has help for usage.

### Radio
Gnuradio flowgraphs including some debugging flowgraphs for running the user segment radio. During the game a single USRP B210 was used to transmit to all eight flatsat B200minis. The usersegment_wideband_tx flowgraph accepts UDP traffic on 8 ports and then transmits packets on the appropriate RF channel after applying BPSK modulation, FEC, and scrambling. Made with Gnuradio 3.9.

The usersegment_wideband_tx flowgraph depends on a few hier flowgraphs under comms/shared/flowgraphs/dependencies
#!/bin/bash

docker inspect fax_raw_1 | grep '"IPAddress": "1'
docker inspect fax_digest1_1 | grep '"IPAddress": "1'
docker inspect fax_digest2_1 | grep '"IPAddress": "1'
docker inspect fax_digest3_1 | grep '"IPAddress": "1'
docker inspect fax_digest4_1 | grep '"IPAddress": "1'
docker inspect fax_digest5_1 | grep '"IPAddress": "1'
docker inspect fax_digest6_1 | grep '"IPAddress": "1'
docker inspect fax_digest7_1 | grep '"IPAddress": "1'
docker inspect fax_digest8_1 | grep '"IPAddress": "1'
#!/bin/bash

docker image build -t fax .

docker run --pids-limit 100 --network="host" -e PYTHONUNBUFFERED=1 fax 2>&1
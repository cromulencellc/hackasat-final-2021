#!/bin/bash

#nc -lu 0.0.0.0 4531 | LD_PRELOAD="/comm/libc-2.28.so" /comm/comm
socat -u udp-listen:4531,ignoreeof,reuseaddr - | LD_PRELOAD="/comm/libc-2.28.so" /comm/comm

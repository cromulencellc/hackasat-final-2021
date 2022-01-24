#!/bin/bash

RESET="\033[0m"
BOLD="\033[1m"
YELLOW="\033[38;5;11m"
RED="\033[31m"

if [ ! -d usersegment ]; then
    echo -e $YELLOW"Creating usersegment venv...\n"$RESET
    python3 -m virtualenv usersegment
    echo -e $YELLOW"Installing requirements in venv...\n"$RESET
    source usersegment/bin/activate
    pip3 install -r requirements.txt
    ln -s usersegment/bin/activate .
    echo $BOLD"Enviroment built. Run 'source activate' to activate venv"$RESET
else
    echo -e $BOLD"Enviroment already ready. Run 'source activate' to activate venv"$RESET
fi
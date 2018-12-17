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

def send(message, channel):
    message = f"{channel} {message} 1;"
    SOCKET.send(message.encode("utf-8"))

def play_chord(*notes):
    for i in range(len(notes) - 1):
        send(notes[i], i)
        time.sleep(NOTE_DELAY)
    send(notes[-1], len(notes) - 1)

def reverse(string):
    return "".join(reversed(string))

buf = []
base_notes = [67, 60, 64, 69]
while True:
    wait = ser.inWaiting()
    if (wait != 0):
        byte = ser.read()
        try:
            buf.append(byte.decode("utf-8"))
        except:
            buf.append(byte)
        # look for "h" delimiter
        if len(buf) > 1 and buf[-1] == "h":
            hello = buf[-1:]
            readings = buf[:-1]
            binary = ["1" + reverse(format(ord(c), '#008b')[2:]) for c in readings]
            print(binary)
            fret_numbers = [c.rindex("1") for c in binary]
            notes = [fret + base for fret, base in zip(fret_numbers, base_notes)]
            print(notes)
            buf = []
            play_chord(*notes)

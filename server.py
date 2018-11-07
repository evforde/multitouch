#!/usr/bin/env python
#
# term.py serial_port port_speed
#

import asyncio
import json
import serial
import sys
import time
import websockets


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


buf = []
async def publish_serial(websocket, path):
    global buf
    name = await websocket.recv()
    print(f"\n\nConnected to {name}")
    while True:
        wait = ser.inWaiting()
        if (wait != 0):
            byte = ser.read()
            try:
                buf.append(byte.decode("utf-8"))
            except:
                buf.append(byte)
            # look for "hello" delimiter
            if len(buf) > 5 and buf[-5:] == ["h", "e", "l", "l", "o"]:
                hello = buf[-5:]
                readings = buf[:-5]
                message = json.dumps([ord(c) for c in readings])
                print(message)
                await websocket.send(message)
                buf = []


asyncio.get_event_loop().run_until_complete(
    websockets.serve(publish_serial, "localhost", 8765))
asyncio.get_event_loop().run_forever()

#!/usr/bin/env python
#
# term.py
#
# term.py serial_port port_speed
#
# Neil Gershenfeld
# CBA MIT 7/27/07
#
# (c) Massachusetts Institute of Technology 2007
# This work may be reproduced, modified, distributed,
# performed, and displayed for any purpose. Copyright is
# retained and must be preserved. The work is provided
# as is; no warranty is provided, and users accept all 
# liability.
#

import sys,time,serial
from select import *


#  check command line arguments
if (len(sys.argv) != 3):
    print("command line: term.py serial_port speed")
    sys.exit()
port = sys.argv[1]
speed = int(sys.argv[2])
look_for_hello = bool(sys.argv[3]) if len(sys.argv) > 3 else False
ser = serial.Serial(port,speed)
ser.setDTR()

# flush buffers
ser.flushInput()
ser.flushOutput()

buffer = []


while True:
    # idle loop
    wait = ser.inWaiting()
    if (wait != 0):
        byte = ser.read()
        print(byte)

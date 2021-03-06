#!/usr/bin/env python

# WS client example - this is purely for testing

import asyncio
import websockets

async def hello():
    async with websockets.connect('ws://localhost:8765') as websocket:
        name = input("What's your name? ")
        await websocket.send(name)
        while True:
            greeting = await websocket.recv()
            print(f"< {greeting}")

asyncio.get_event_loop().run_until_complete(hello())

PROJECT=multitouch
HEX=$(PROJECT).hex
OUT=$(PROJECT).obj

RECEIVER=multitouch
HEX_RECEIVER=$(RECEIVER).hex
OUT_RECEIVER=$(RECEIVER).obj

SOURCES=multitouch.c serial.c
OBJECTS=$(SOURCES:.c=.o)
SOURCES_RECEIVER=receiver.c serial.c
OBJECTS_RECEIVER=$(SOURCES_RECEIVER:.c=.o)

MMCU=attiny44
F_CPU=20000000
CC=avr-gcc

CFLAGS=-mmcu=$(MMCU) -Wall -Os -DF_CPU=$(F_CPU)

$(HEX): $(OUT)
	avr-objcopy -j .text -O ihex $(OUT) $(HEX)
	avr-size --mcu=$(MMCU) --format=avr $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJECTS)

$(HEX_RECEIVER): $(OUT_RECEIVER)
	avr-objcopy -j .text -O ihex $(OUT_RECEIVER) $(HEX_RECEIVER)
	avr-size --mcu=$(MMCU) --format=avr $(OUT_RECEIVER)

$(OUT_RECEIVER): $(OBJECTS_RECEIVER)
	$(CC) $(CFLAGS) -o $(OUT_RECEIVER) $(OBJECTS_RECEIVER)

# Dependency tree:
multitouch.c: serial.h macros.h
receiver.c: serial.h macros.h
serial.o: serial.c serial.h

clean:
	rm *.o *.hex
################################################################################

program: programmed program-usbtiny
	touch programmed

program-receiver: programmed-receiver program-usbtiny-receiver
	touch programmed

fuse: fuse-usbtiny

programmed: $(HEX)

programmed-receiver: $(HEX_RECEIVER)

program-avrisp2: $(HEX)
	avrdude -p $(MMCU) -P usb -c avrisp2 -U flash:w:$(HEX)

fuse-avrisp2:
	avrdude -p $(MMCU) -P usb -c avrisp2 -U lfuse:w:0x5E:m

program-usbtiny: $(HEX)
	avrdude -p $(MMCU) -P usb -c usbtiny -U flash:w:$(HEX)

program-usbtiny-receiver: $(HEX_RECEIVER)
	avrdude -p $(MMCU) -P usb -c usbtiny -U flash:w:$(HEX_RECEIVER)

fuse-usbtiny:
	avrdude -p $(MMCU) -P usb -c usbtiny -U lfuse:w:0x5E:m

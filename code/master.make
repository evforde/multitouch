PROJECT=master
HEX=$(PROJECT).hex
OUT=$(PROJECT).obj

SOURCES=master.c serial.c multitouch.c
OBJECTS=$(SOURCES:.c=.o)

MMCU=atmega328p
F_CPU=16000000
CC=avr-gcc

CFLAGS=-mmcu=$(MMCU) -Wall -Os -DF_CPU=$(F_CPU)

all: $(HEX)

$(HEX): $(OUT)
	avr-objcopy -j .text -O ihex $(OUT) $(HEX)
	avr-size --mcu=$(MMCU) --format=avr $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJECTS)

# Dependency tree:
master.c: serial.h macros.h multitouch.h
serial.o: serial.c serial.h
multitouch.o: multitouch.c multitouch.h

clean:
	rm *.o *.hex
################################################################################

program: programmed program-usbtiny
	@echo "success!"

fuse: fuse-usbtiny

programmed: $(HEX)

program-avrisp2: $(HEX)
	avrdude -p $(MMCU) -P usb -c avrisp2 -U flash:w:$(HEX)

fuse-avrisp2:
	avrdude -p $(MMCU) -P usb -c avrisp2 -U lfuse:w:0x5E:m

program-usbtiny: $(HEX)
	avrdude -p $(MMCU) -P usb -c usbtiny -U flash:w:$(HEX)

fuse-usbtiny:
	avrdude -p $(MMCU) -P usb -c usbtiny -U lfuse:w:0x5E:m

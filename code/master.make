PROJECT=receiver
HEX=$(PROJECT).hex
OUT=$(PROJECT).obj

SOURCES=receiver.c serial.c
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
receiver.c: serial.h macros.h
serial.o: serial.c serial.h

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

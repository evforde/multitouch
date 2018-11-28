PROJECT=multitouch
HEX=$(PROJECT).hex
OUT=$(PROJECT).obj

SOURCES=multitouch.c serial.c
OBJECTS=$(SOURCES:.c=.o)

MMCU=atmega328p
F_CPU = 16000000
CC=avr-gcc

CFLAGS=-mmcu=$(MMCU) -Wall -Os -DF_CPU=$(F_CPU)

$(HEX): $(OUT)
	avr-objcopy -j .text -O ihex $(OUT) $(HEX)
	avr-size --mcu=$(MMCU) --format=avr $(OUT)

$(OUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJECTS)


# Dependency tree:
multitouch.c: serial.h macros.h
serial.o: serial.c serial.h

clean:
	rm $(OBJECTS) $(OUT) $(HEX)
################################################################################

program: programmed program-usbtiny
	touch programmed

fuse: fuse-usbtiny

programmed: $(HEX)

program-avrisp2: $(HEX)
	avrdude -p atmega328p -P usb -c avrisp2 -U flash:w:$(HEX)

fuse-avrisp2:
	avrdude -p atmega328p -P usb -c avrisp2 -U lfuse:w:0x7E:m

program-usbtiny: $(HEX)
	avrdude -p atmega328p -P usb -c usbtiny -U flash:w:$(HEX)

fuse-usbtiny:
	avrdude -p atmega328p -P usb -c usbtiny -U lfuse:w:0x7E:m

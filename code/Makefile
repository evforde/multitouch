PROJECT=multitouch
HEX=$(PROJECT).hex
OUT=$(PROJECT).obj

SOURCES=multitouch.c serial.c
OBJECTS=$(SOURCES:.c=.o)

MMCU=attiny44
F_CPU = 20000000
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

program-bsd: $(HEX)
	avrdude -p attiny44 -c bsd -U flash:w:$(HEX)

program-dasa: $(HEX)
	avrdude -p attiny44 -P /dev/ttyUSB0 -c dasa -U flash:w:$(HEX)

program-avrisp2: $(HEX)
	avrdude -p atmega168 -P usb -c avrisp2 -U flash:w:$(HEX)

program-usbtiny: $(HEX)
	avrdude -p attiny44 -P usb -c usbtiny -U flash:w:$(HEX)

fuse-usbtiny:
	avrdude -p attiny44 -P usb -c usbtiny -U lfuse:w:0x7E:m

program-dragon: $(HEX)
	avrdude -p attiny44 -P usb -c dragon_isp -U flash:w:$(HEX)

PROJECT=multitouch
HEX=$(PROJECT).hex
OUT=$(PROJECT).obj

SOURCES=multitouch.c serial.c
OBJECTS=$(SOURCES:.c=.o)

MMCU=attiny44
F_CPU = 20000000
CC=avr-gcc

CFLAGS=-mmcu=$(MMCU) -Wall -Os -DF_CPU=$(F_CPU)

all: $(HEX)
	@echo -e "\n\033[0;31mMAKE SURE YOU UPDATED THE ADDRESS FOR THIS SLAVE\033[0m\n"

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

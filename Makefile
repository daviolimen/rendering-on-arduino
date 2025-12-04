# simple makefile to compile example and flash to atmega328p

EXAMPLES = examples
INCLUDE  = include
BUILD    = build

CC = avr-gcc
OC = avr-objcopy
AVRDUDE = avrdude

CFLAGS  = -mmcu=atmega328p -Os -Wall -I$(INCLUDE)
LDFLAGS = -mmcu=atmega328p

%:
	@rm -rf $(BUILD)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -c $(EXAMPLES)/$*.c -o $(BUILD)/$*.o
	$(CC) $(LDFLAGS) $(BUILD)/$*.o -o $(BUILD)/$*.elf
	$(OC) -O ihex -R .eeprom $(BUILD)/$*.elf $(BUILD)/$*.hex
	@echo "Built: $(BUILD)/$*.hex"

install:
	@hex=$$(ls $(BUILD)/*.hex 2>/dev/null | head -n 1); \
	if [ -z "$$hex" ]; then echo "Nothing to install. Build first."; exit 1; fi; \
	echo "Flashing $$hex..."; \
	$(AVRDUDE) -c arduino -p atmega328p -b 115200 -P /dev/ttyUSB0 -U flash:w:$$hex:i

clean:
	rm -rf $(BUILD)

#ifndef DISPLAY_H
#define DISPLAY_H

#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

// some defines for common operations
#define RST_PIN 0
#define DC_PIN 1

#define SLAVE_SELECT PORTB &= ~(1 << PB2)
#define SLAVE_DESELECT PORTB |= (1 << PB2)

#define DC_COMM PORTB &= ~(1 << DC_PIN)
#define DC_DATA PORTB |= (1 << DC_PIN)

#define RST_LOW PORTB &= ~(1 << RST_PIN)
#define RST_HIGH PORTB |= (1 << RST_PIN)

// defines for the display commands, taken from the Adafruit ST77xx repository
#define DELAY 0x80

#define NOP 0x00
#define SWRESET 0x01
#define RDDID 0x04
#define RDDST 0x09

#define SLPIN 0x10
#define SLPOUT 0x11
#define PTLON 0x12
#define NORON 0x13

#define INVOFF 0x20
#define INVON 0x21
#define DISPOFF 0x28
#define DISPON 0x29
#define CASET 0x2A
#define RASET 0x2B
#define RAMWR 0x2C
#define RAMRD 0x2E

#define PTLAR 0x30
#define TEOFF 0x34
#define TEON 0x35
#define MADCTL 0x36
#define COLMOD 0x3A

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00

#define RDID1 0xDA
#define RDID2 0xDB
#define RDID3 0xDC
#define RDID4 0xDD

// initialization sequence for the display
static const uint8_t PROGMEM initSequence[] = {
	9,
	SWRESET, DELAY, 150,
	SLPOUT, DELAY, 10,
	COLMOD, 1+DELAY, 0x55, 10,
	MADCTL, 1, MADCTL_MX | MADCTL_MY | MADCTL_RGB,
	CASET, 4, 0, 0, 239>>8, 239&0xff,
	RASET, 4, 0, 0, 319>>8, 319&0xff,
	INVON, DELAY, 10,
	NORON, DELAY, 10,
	DISPON, DELAY, 10
};

void delayHelper(uint16_t ms) {
	for (int i = 0; i < ms; i++) _delay_ms(1);
}

// function to setup the SPI pins and registers
void spiSetup(void) {
	DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
	SLAVE_DESELECT;
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X); // maximum possible SPI clock speed
}

void displaySetup(void) {
	spiSetup();
	DDRB |= (1 << RST_PIN) | (1 << DC_PIN);
	RST_LOW;
	_delay_ms(20);
	RST_HIGH;
	_delay_ms(150);
}

// function to send data through SPI
void spiWrite(uint8_t byte) {
	SPDR = byte;
	while (!(SPSR & (1 << SPIF)));
}

// function to send command and arguments from PROGMEM
void sendCommandFromPgm(uint8_t comm, const uint8_t *addr, uint8_t numArgs) {
	SLAVE_SELECT;
	DC_COMM;
	spiWrite(comm);

	DC_DATA;
	for (uint8_t i = 0; i < numArgs; i++) {
		spiWrite(pgm_read_byte(addr++));
	}
	SLAVE_DESELECT;
}

void sendCommand(uint8_t comm, uint8_t *addr, uint8_t numArgs) {
	SLAVE_SELECT;
	DC_COMM;
	spiWrite(comm);

	DC_DATA;
	for (uint8_t i = 0; i < numArgs; i++) {
		spiWrite(*addr++);
	}
	SLAVE_DESELECT;
}


// function to initialize the display, with the same logic as the Adafruit one
void initDisplayHelper(const uint8_t *addr) {
	uint8_t numCommands = pgm_read_byte(addr++);
	while (numCommands--) {
		uint8_t comm = pgm_read_byte(addr++);
		uint8_t numArgs = pgm_read_byte(addr++);

		uint16_t delay = numArgs & DELAY;
		numArgs &= ~DELAY;
		sendCommandFromPgm(comm, addr, numArgs);
		addr += numArgs;

		if (delay) {
			delay = pgm_read_byte(addr++);
			if (delay == 255) {
				delay = 500;
			}
			delayHelper(delay);
		}
	}
}

void initDisplay(void) {
	initDisplayHelper(initSequence);
}

// function to set the window
void setWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	uint8_t casetArgs[] = {x0 >> 8, x0, x1 >> 8, x1};
	uint8_t rasetArgs[] = {y0 >> 8, y0, y1 >> 8, y1};
	sendCommand(CASET, casetArgs, 4);
	sendCommand(RASET, rasetArgs, 4);
	sendCommand(RAMWR, 0, 0);
}

#endif

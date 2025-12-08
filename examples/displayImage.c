// this example should display the image from the utils directory via UART

#define F_CPU 16000000L
#define BAUD 115200

#include <avr/io.h>
#include <util/setbaud.h>
#include "display.h"

void uartInit(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

uint8_t uartRead(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void uartWrite(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void setup(void) {
    displaySetup();
    initDisplay();
    uartInit();
}

uint8_t buffer[480];

void loop(void) {
    setWindow(0, 0, 239, 319);
    SLAVE_SELECT;
    DC_DATA;
    for (uint16_t col = 0; col < 320; col++) {
        for (uint16_t row = 0; row < 480; row++) {
            buffer[row] = uartRead(); 
        }
        for (uint16_t row = 0; row < 480; row++) {
            spiWrite(buffer[row]);
        }
    }
    SLAVE_DESELECT;
}

void main(void) {
    setup();
    while (1) {
        loop();
    }
}
// This program is to test if the display powers on and if the orientation is correct
// It should render four squares of colors red, blue, yellow and green
#include "display.h"

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE 0x001F
#define COLOR_CYAN 0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW 0xFFE0
#define COLOR_ORANGE 0xFC00

void setup(void) {
    displaySetup();
    initDisplay();
}

void loop(void) {
    setWindow(0, 0, 239, 319);
    SLAVE_SELECT;
    DC_DATA;
    for (uint32_t row = 0; row < 320; row++) {
        for (uint32_t col = 0; col < 240; col++) {
            uint16_t color;
            if (row < 160) {
                if (col < 120) color = COLOR_RED;
                else color = COLOR_BLUE;
            } else {
                if (col < 120) color = COLOR_YELLOW;
                else color = COLOR_GREEN;
            }
            spiWrite(color >> 8);
            spiWrite(color);
        }
    }
    SLAVE_DESELECT;
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
}
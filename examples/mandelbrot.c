// This example displays the mandelbrot set

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

// Here I am using fixed-point arithmetic to speed things up a bit
// I chose to use 22 bits integer and 10 bits fractional
// It could be done with 16-bit integer, but the quality was worse, so I kept it at 32-bit
// It's pretty damn slow, better come back here later
#define FP_SHIFT 10
#define FP_ONE (1 << FP_SHIFT)
#define FP(x) ((int32_t) ((x) * FP_ONE))
#define FP_MUL(a,b) ((int32_t) (((int64_t)(a) * (b)) >> FP_SHIFT))

#define WIDTH 3
#define HEIGHT 2
#define MINX -2
#define MAXY 1

uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue) {
    uint16_t color = (uint16_t) (red << 11) | (green << 5) | blue;
    return color;
}

void loop(void) {
    setWindow(0, 0, 239, 319);
    SLAVE_SELECT;
    DC_DATA;
    for (uint32_t col = 0; col < 320; col++) {
        for (uint32_t row = 0; row < 240; row++) {
            int32_t x = FP(MINX) + (int32_t) FP(col) * WIDTH / 320;
            int32_t y = FP(MAXY) - (int32_t) FP(row) * HEIGHT / 240;
            int32_t a = 0, b = 0;
            
            uint8_t n;
            for (n = 0; n < 30; n++) {
                int32_t a2 = FP_MUL(a,a);
                int32_t b2 = FP_MUL(b,b);
                if (a2 + b2 > FP(4)) break;
                b = FP_MUL(FP(2), FP_MUL(a,b)) + y;
                a = a2 - b2 + x;
            }

            uint32_t factor = FP(n) / 30;
            uint16_t color = getColor((factor * 32) >> FP_SHIFT, (factor * 64) >> FP_SHIFT, (factor * 32) >> FP_SHIFT);
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


#include <stdint.h>
#include "config.h"
#include "graphics.h"

// VGA framebuffer
#define VGA_BUFFER ((volatile uint8_t *)0x08000000)

// Rita en pixel på skärmen
void rita_pixel(int x, int y, uint8_t farg)
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;

    VGA_BUFFER[y * SCREEN_WIDTH + x] = farg;
}

// Gör hela skärmen svart
void clear_screen(void)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            VGA_BUFFER[y * SCREEN_WIDTH + x] = SVART;
        }
    }
}

// Draw the paddles and the ball
void rita_fyrkant(int x, int y, size_x, size_y, uint8_t farg)
{
    for (int dy = 0; dy < size_y; dy++) {
        for (int dx = 0; dx < size_x; dx++) {
            rita_pixel(x + dx, y + dy, farg);
        }
    }
}

// Rita bara delen som har flyttats
void uppdatera_paddel(int x, int old_y, int new_y)
{
    if (new_y > old_y) {
        for (int y = old_y; y < new_y; y++)
            for (int xoff = 0; xoff < PADDLE_WIDTH; xoff++)
                rita_pixel(x + xoff, y, SVART);

        for (int y = old_y + PADDLE_HEIGHT;
             y < new_y + PADDLE_HEIGHT; y++)
            for (int xoff = 0; xoff < PADDLE_WIDTH; xoff++)
                rita_pixel(x + xoff, y, VITT);
    } else if (new_y < old_y) {
        for (int y = new_y + PADDLE_HEIGHT;
             y < old_y + PADDLE_HEIGHT; y++)
            for (int xoff = 0; xoff < PADDLE_WIDTH; xoff++)
                rita_pixel(x + xoff, y, SVART);

        for (int y = new_y; y < old_y; y++)
            for (int xoff = 0; xoff < PADDLE_WIDTH; xoff++)
                rita_pixel(x + xoff, y, VITT);
    }
}

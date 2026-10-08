
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

// Draw the paddle
void rita_paddel(int x, int y, uint8_t farg)
{
    for (int dy = 0; dy < PADDLE_HEIGHT; dy++) {
        for (int dx = 0; dx < PADDLE_WIDTH; dx++) {
            rita_pixel(x + dx, y + dy, farg);
        }
    }
}

// Rita bollen
void rita_boll(int x, int y, uint8_t farg)
{
    for (int y_skillnad = 0; y_skillnad < BALL_SIZE; y_skillnad++) {
        for (int x_skillnad = 0; x_skillnad < BALL_SIZE; x_skillnad++) {
            rita_pixel(x + x_skillnad, y + y_skillnad, farg);
        }
    }
}

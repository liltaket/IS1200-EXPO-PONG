#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

void clear_screen(void);
void rita_pixel(int x, int y, uint8_t farg);
void rita_paddel(int x, int y, uint8_t farg);
void rita_boll(int x, int y, uint8_t farg);

#endif

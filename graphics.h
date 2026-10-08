//Hampus
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

void clear_screen(void);
void rita_pixel(int x, int y, uint8_t farg);
void rita_fyrkant(int x, int y, int size_x, int size_y, uint8_t farg);
void uppdatera_paddel(int x, int old_y, int new_y);

#endif

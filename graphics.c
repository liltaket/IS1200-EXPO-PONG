#include <stdint.h>
#include "config.h"

#define VGA_BUFFER ((volatile uint8_t *)0x08000000) //Skapar en array till framebuffern som är en pekare till adressen 0x08000000 och har 8 bitar per pixel 


// Räknar ut vilken plats i minnet som motsvara pixeln kordinat och sätter färgen på den pixeln
void rita_pixel(int x, int y, uint8_t farg){
    
    if(x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    VGA_BUFFER[y * SCREEN_WIDTH + x] = farg;  // Offseten till pixlarna i minnet
}


//Gör hela skärmen svart, skärmen uppdaterar varje frame, uppdaterar den hela tiden
void clear_screen(void)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++) {

        for (int x = 0; x < SCREEN_WIDTH; x++) {

            VGA_BUFFER[y * SCREEN_WIDTH + x] = SVART;
        }

    }
}

//Ritar en rektangel på skärmen 
void rita_paddel(int x, int y, uint8_t farg)
{
    for (int dy = 0; dy < PADDLE_HEIGHT; dy++) {

        for (int dx = 0; dx < PADDLE_WIDTH; dx++) {

            rita_pixel(x + dx, y + dy, farg);
        }

    }

}

//Ritar bollen på skärmen
void rita_boll(int x, int y, uint8_t farg)
{
    for (int y_skillnad = 0; y_skillnad < BALL_SIZE; y_skillnad++) {

        for (int x_skillnad = 0; x_skillnad < BALL_SIZE; x_skillnad++) {

            rita_pixel(x + x_skillnad, y + y_skillnad, farg);
        }

    }

}

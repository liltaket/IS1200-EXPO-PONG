#include <stdint.h>

#define VGA_BUFFER ((volatile uint8_t *)0x08000000) //Skapar en array till framebuffern som är en pekare till adressen 0x08000000 och har 8 bitar per pixel 

// Hur många pixlar på skärmen
#define BREDDEN  320  
#define HOJDEN 240  

// Storlek på paddlar och boll
#define PADDLE_BREDD 4
#define PADDLE_HOJD  40
#define BOLL_STORLEK 5


// Använder RRR GGG BB  
#define SVART 0x00 
#define VITT 0xFF


// Räknar ut vilken plats i minnet som motsvara pixeln kordinat och sätter färgen på den pixeln
void rita_pixel(int x, int y, uint8_t farg){
    if(x < 0) return;

    if(x >= BREDDEN) return;

    if(y < 0) return;

    if(y >= HOJDEN) return;

    VGA_BUFFER[y * BREDDEN + x] = farg;  // Offseten till pixlarna i minnet
}


//Gör hela skärmen svart, skärmen uppdaterar varje frame, uppdaterar den hela tiden
void clear_screen(void)
{
    for (int y = 0; y < HOJDEN; y++) {

        for (int x = 0; x < BREDDEN; x++) {

            VGA_BUFFER[y * BREDDEN + x] = SVART;
        }

    }
}

//Ritar en rektangel på skärmen 
void rita_paddel(int x, int y)
{
    for (int dy = 0; dy < PADDLE_HOJD; dy++) {

        for (int dx = 0; dx < PADDLE_BREDD; dx++) {

            rita_pixel(x + dx, y + dy, VITT);
        }

    }

}

//Ritar bollen på skärmen
void rita_boll(int x, int y)
{
    for (int y_skillnad = 0; y_skillnad < BOLL_STORLEK; y_skillnad++) {

        for (int x_skillnad = 0; x_skillnad < BOLL_STORLEK; x_skillnad++) {

            rita_pixel(x + x_skillnad, y + y_skillnad, VITT);
        }

    }

}

/* main.c

   Based on IS1200 Lab 3 code.
   Made by Bruno & Hampus
*/

#include <stdint.h>
#include "config.h"

void clear_screen(void);
void rita_paddel(int x, int y);
void rita_boll(int x, int y);

/* Switches */
#define SWITCHES (*(volatile uint32_t *)0x04000010)


/*
 * boot.S förväntar sig att denna funktion finns.
 * Vi använder inga interrupts i spelet nu.
 */
void handle_interrupt(unsigned cause)
{
    (void)cause;
}


/* 7-segment digits */
static const int digits[10] = {
    0xC0, // 0
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90  // 9
};


/* Game positions */

int left_x = 10;
int left_y = 100;

int right_x = SCREEN_WIDTH - PADDLE_WIDTH - 10;
int right_y = 100;

int ball_x = SCREEN_WIDTH / 2;
int ball_y = SCREEN_HEIGHT / 2;

int ball_dx = BALL_SPEED_X;
int ball_dy = BALL_SPEED_Y;

static int left_score = 0;
static int right_score = 0;


/*
 * Delay styr spelets hastighet.
 *
 * Större nummer = långsammare spel
 * Mindre nummer = snabbare spel
 */
static void delay(void)
{
    for (volatile int i = 0; i < 50000; i++) {
    }
}


/* Write one digit to one of the six 7-segment displays. */
void set_displays(int display_number, int value)
{
    volatile int *display =
        (volatile int *)(0x04000050 + display_number * 0x10);

    *display = digits[value];
}


/* Read switches */
static uint32_t read_switches(void)
{
    return SWITCHES;
}


/* Move paddles */
static void move_paddles(uint32_t sw)
{
    /* Left paddle up */
    if (sw & (1u << 8))
        left_y -= PADDLE_SPEED;

    /* Left paddle down */
    if (!(sw & (1u << 9)))
        left_y += PADDLE_SPEED;

    /* Right paddle up */
    if (sw & (1u << 1))
        right_y -= PADDLE_SPEED;

    /* Right paddle down */
    if (!(sw & (1u << 0)))
        right_y += PADDLE_SPEED;
}


/* Keep paddles inside screen */
static void clamp_paddles(void)
{
    if (left_y < 0)
        left_y = 0;

    if (left_y > SCREEN_HEIGHT - PADDLE_HEIGHT)
        left_y = SCREEN_HEIGHT - PADDLE_HEIGHT;

    if (right_y < 0)
        right_y = 0;

    if (right_y > SCREEN_HEIGHT - PADDLE_HEIGHT)
        right_y = SCREEN_HEIGHT - PADDLE_HEIGHT;
}


/* Move ball */
static void move_ball(void)
{
    ball_x += ball_dx;
    ball_y += ball_dy;
}


/* Check top and bottom wall */
static void check_wall_collision(void)
{
    if (ball_y <= 0) {
        ball_y = 0;
        ball_dy = -ball_dy;
    }

    if (ball_y + BALL_SIZE >= SCREEN_HEIGHT) {
        ball_y = SCREEN_HEIGHT - BALL_SIZE;
        ball_dy = -ball_dy;
    }
}


/* Check paddle collision */
static void check_paddle_collision(void)
{
    /* Left paddle */
    if (ball_dx < 0 &&
        ball_x <= left_x + PADDLE_WIDTH &&
        ball_x + BALL_SIZE >= left_x &&
        ball_y + BALL_SIZE >= left_y &&
        ball_y <= left_y + PADDLE_HEIGHT) {

        ball_x = left_x + PADDLE_WIDTH;
        ball_dx = -ball_dx;
    }


    /* Right paddle */
    if (ball_dx > 0 &&
        ball_x + BALL_SIZE >= right_x &&
        ball_x <= right_x + PADDLE_WIDTH &&
        ball_y + BALL_SIZE >= right_y &&
        ball_y <= right_y + PADDLE_HEIGHT) {

        ball_x = right_x - BALL_SIZE;
        ball_dx = -ball_dx;
    }
}


/* Reset ball to middle */
static void reset_ball()
{
   static uint8_t rng = 73;
   rng = (uint8_t)(rng * 17u + 43u);

    ball_x = SCREEN_WIDTH / 2;
    ball_y = SCREEN_HEIGHT / 2;

    if (rng & 0x80) 
    {
       ball_dx = BALL_SPEED_X;
    }
    else
    {
       ball_dx = -BALL_SPEED_X;
    }

}

static void reset_score()
{
   left_score = 0;
   right_score = 0;
   set_displays(0, 0);
   set_displays(1, 0);
   set_displays(4, 0);
   set_displays(5, 0);
}


/* Check if someone scored */
static void check_goal(void)
{
    /* Ball leaves left side -> right player scores */
    if (ball_x < 0) {

        right_score++;

        if (right_score > 99)
            right_score = 0;

        set_displays(0, right_score % 10);
        set_displays(1, (right_score / 10) % 10);

        reset_ball();
    }


    /* Ball leaves right side -> left player scores */
    if (ball_x + BALL_SIZE >= SCREEN_WIDTH) {

        left_score++;

        if (left_score > 99)
            left_score = 0;

        set_displays(4, left_score % 10);
        set_displays(5, (left_score / 10) % 10);

        reset_ball();
    }
}


/* Draw game */
static void draw_game(void)
{
    clear_screen();

    rita_paddel(left_x, left_y);
    rita_paddel(right_x, right_y);
    rita_boll(ball_x, ball_y);
}


/* Main */
int main(void)
{
    /* Start score */
    set_displays(0, 0);
    set_displays(1, 0);
    set_displays(4, 0);
    set_displays(5, 0);

    clear_screen();


    while (1)
    {
       uint32_t sw = read_switches();

       if (sw & (1u << 6))
       {
          reset_score();
       }

       if (sw & (1u << 7)) 
       {
          delay();
          continue;
       }

        move_paddles(sw);
        clamp_paddles();

        move_ball();

        check_wall_collision();
        check_paddle_collision();
        check_goal();

        draw_game();

        /*
         * Vänta innan nästa frame.
         * Detta ersätter timer interrupt.
         */
        delay();
    }


    return 0;
}

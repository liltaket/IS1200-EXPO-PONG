/* main.c

   Based on IS1200 Lab 3 code.
   Made by Bruno & Hampus
*/
#include <stdint.h>
#include "config.h"

void clear_screen(void);
void rita_paddel(int x, int y);
void rita_boll(int x, int y);

//interupt grejer
volatile int timeoutcount = 0;
extern void enable_interrupt(void);

/* Switches */

#define SWITCHES (*(volatile uint32_t *)0x04000010)



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




void timer_ack(void);


/* Called when an interrupt is triggered. */
void handle_interrupt(unsigned cause)
{
    (void)cause;

    timer_ack();

    timeoutcount++;

    /*
     * Pong timer logic will go here later.
     */
}


/* Initialize timer interrupts. */
void labinit(void)
{
    volatile unsigned int *timer_status  =
        (volatile unsigned int *)0x04000020;

    volatile unsigned int *timer_control =
        (volatile unsigned int *)0x04000024;

    volatile unsigned int *timer_periodl =
        (volatile unsigned int *)0x04000028;

    volatile unsigned int *timer_periodh =
        (volatile unsigned int *)0x0400002C;

    *timer_status = 0;

    *timer_periodl = 0xC6BF;
    *timer_periodh = 0x002D;

    /* Start + continuous + interrupt */
    *timer_control = 0x7;

    enable_interrupt();
}


/* Write one digit to one of the six 7-segment displays. */
void set_displays(int display_number, int value)
{
    volatile int *display =
        (volatile int *)(0x04000050 + display_number * 0x10);

    *display = digits[value];
}


/* Clear timer interrupt flag. */
void timer_ack(void)
{
    volatile unsigned int *timer_status =
        (volatile unsigned int *)0x04000020;

    *timer_status = 0;
}

//functions for the game

static uint32_t read_switches(void)
{
    return SWITCHES;
}

static void move_paddles(uint32_t sw)
{
    if (sw & (1u << 8))
        left_y -= PADDLE_SPEED;

    if (sw & (1u << 9))
        left_y += PADDLE_SPEED;

    if (sw & (1u << 1))
        right_y -= PADDLE_SPEED;

    if (sw & (1u << 0))
        right_y += PADDLE_SPEED;
}

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

static void move_ball(void)
{
    ball_x += ball_dx;
    ball_y += ball_dy;
}

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

static void check_paddle_collision(void)
{
    // Left paddle
    if (ball_dx < 0 &&
        ball_x <= left_x + PADDLE_WIDTH &&
        ball_x + BALL_SIZE >= left_x &&
        ball_y + BALL_SIZE >= left_y &&
        ball_y <= left_y + PADDLE_HEIGHT) {

        ball_x = left_x + PADDLE_WIDTH;
        ball_dx = -ball_dx;
    }

    // Right paddle
    if (ball_dx > 0 &&
        ball_x + BALL_SIZE >= right_x &&
        ball_x <= right_x + PADDLE_WIDTH &&
        ball_y + BALL_SIZE >= right_y &&
        ball_y <= right_y + PADDLE_HEIGHT) {

        ball_x = right_x - BALL_SIZE;
        ball_dx = -ball_dx;
    }
}

static void reset_ball(int direction)
{
    ball_x = SCREEN_WIDTH / 2;
    ball_y = SCREEN_HEIGHT / 2;

    ball_dx = direction * BALL_SPEED_X;
    ball_dy = BALL_SPEED_Y;
}

static void check_goal(void)
{
    // Ball left the left side means right player scores 
    if (ball_x < 0) {
        right_score++;
        reset_ball(1);
    }

    // Ball left the right side means left player scores
    if (ball_x + BALL_SIZE >= SCREEN_WIDTH) {
        left_score++;
        reset_ball(-1);
    }
}

static void draw_game(void)
{
    clear_screen();

    rita_paddel(left_x, left_y);
    rita_paddel(right_x, right_y);
    rita_boll(ball_x, ball_y);
}







int main(void)
{
    labinit();
    clear_screen();
    while (1) 
    {
        if (timeoutcount == 0)
            continue;

        timeoutcount--;    

        uint32_t sw = read_switches();

        move_paddles(sw);
        clamp_paddles();

        move_ball();
        check_wall_collision();
        check_paddle_collision();
        check_goal();

        draw_game();
    }

    return 0;
}

/* main.c

   Based on IS1200 Lab 3 code.
   Made by Bruno & Hampus
*/

#include <stdint.h>
#include "config.h"
#include "graphics.h"

#define TIMER_STATUS  (*(volatile uint32_t *)0x04000020)
#define TIMER_CONTROL (*(volatile uint32_t *)0x04000024)
#define TIMER_PERIODL (*(volatile uint32_t *)0x04000028)
#define TIMER_PERIODH (*(volatile uint32_t *)0x0400002C)


static void init_timer(void)
{
    uint32_t period = 300000; // 10 ms vid 30 MHz

    TIMER_CONTROL = 8; // Stoppa timern
    TIMER_PERIODL = (period - 1) & 0xFFFF;
    TIMER_PERIODH = (period - 1) >> 16;
    TIMER_STATUS = 0;
    TIMER_CONTROL = 6; // Start + continuous
}

// Switcharna
#define SWITCHES (*(volatile uint32_t *)0x04000010)

// Siffror till 7-segment - LAB 3 CODE Bruno
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

// Startpositioner och poäng tilsammans hittade värden
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

// Krävs av boot.S, men vi kör utan interrupts

void handle_interrupt(unsigned cause) //old
{
    (void)cause; //berättar för kompilatorn att vi inte använder cause med flit
}

// Små hjälpfunktioner

// dealy med timern 
static void delay(void)
{
    while (!(TIMER_STATUS & 1)) {}
    TIMER_STATUS = 0;
}

// Skriv en siffra på displayen
void set_displays(int display_number, int value) // Hampus
{
    volatile int *display = (volatile int *)(0x04000050 + display_number * 0x10);

    if (value == -1)
        *display = 0xFF;
    else
        *display = digits[value];
}

// Läs av switcharna
static uint32_t read_switches(void) //old / Hampus
{
    return SWITCHES;
}

// Flytta paddlar och boll

// Spelarnas knappar flyttar paddlarna
static void move_paddles(uint32_t sw) //Hampus
{
    if (sw & (1u << 8))
        left_y -= PADDLE_SPEED;

    if (!(sw & (1u << 9)))
        left_y += PADDLE_SPEED;

    if (sw & (1u << 1))
        right_y -= PADDLE_SPEED;

    if (!(sw & (1u << 0)))
        right_y += PADDLE_SPEED;
}

// Håll paddlarna inom skärmen
static void clamp_paddles(void) //Hampus
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

// Move the ball
static void move_ball(void) //Hampus
{
    ball_x += ball_dx;
    ball_y += ball_dy;
}


// Kolla krockar

// Studsa mot tak och golv
static void check_wall_collision(void) //Hampus
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

// Studsa mot paddlarna
static void check_paddle_collision(void) //Hampus
{
    if (ball_dx < 0 &&
        ball_x <= left_x + PADDLE_WIDTH &&
        ball_x + BALL_SIZE >= left_x &&
        ball_y + BALL_SIZE >= left_y &&
        ball_y <= left_y + PADDLE_HEIGHT) {
        ball_x = left_x + PADDLE_WIDTH;
        ball_dx = -ball_dx;
    }

    if (ball_dx > 0 &&
        ball_x + BALL_SIZE >= right_x &&
        ball_x <= right_x + PADDLE_WIDTH &&
        ball_y + BALL_SIZE >= right_y &&
        ball_y <= right_y + PADDLE_HEIGHT) {
        ball_x = right_x - BALL_SIZE;
        ball_dx = -ball_dx;
    }
}

// Poäng och omstart

// Ny boll i mitten, slumpa riktning
static void reset_ball() //Bruno
{
    static uint8_t rng = 73;
    rng = (uint8_t)(rng * 17u + 43u);

    ball_x = SCREEN_WIDTH / 2;
    ball_y = SCREEN_HEIGHT / 2;

    if (rng & 0x80) {
        ball_dx = BALL_SPEED_X;
    } else {
        ball_dx = -BALL_SPEED_X;
    }
}

static void update_score_display(int score, int display) //Bruno
{
    set_displays(display, score % 10);
    set_displays(display + 1, (score / 10) % 10);
}

// Börja om poängen
static void reset_score()//Bruno
{
    left_score = 0;
    right_score = 0;
    update_score_display(0, 0);
    update_score_display(0, 4);
}

// Ge poäng när bollen går ut
static void check_goal(void)//Bruno
{
    if (ball_x < 0) {
        right_score++;

        if (right_score > 99)
            right_score = 0;

        update_score_display(right_score, 0);

        reset_ball();
    }

    if (ball_x + BALL_SIZE >= SCREEN_WIDTH) {
        left_score++;

        if (left_score > 99)
            left_score = 0;

        update_score_display(left_score, 4);
        reset_ball();
    }
}

static void uppdatera_boll(void) //Bruno
{
    int old_x = ball_x;
    int old_y = ball_y;

    move_ball();
    check_wall_collision();
    check_paddle_collision();
    check_goal();

    int dx = ball_x - old_x;
    int dy = ball_y - old_y;

    // Om bollen hoppat till en helt ny position
    if (dx >= BALL_SIZE || dx <= -BALL_SIZE ||
        dy >= BALL_SIZE || dy <= -BALL_SIZE) {
        rita_fyrkant(old_x, old_y, BALL_SIZE, BALL_SIZE, SVART);
    }
    else {
        // Sudda kanten i X-led
        if (dx > 0)
            rita_fyrkant(old_x, old_y, dx, BALL_SIZE, SVART);
        else if (dx < 0)
            rita_fyrkant(old_x + BALL_SIZE + dx, old_y, -dx, BALL_SIZE, SVART);

        // Sudda kanten i Y-led
        if (dy > 0)
            rita_fyrkant(old_x, old_y, BALL_SIZE, dy, SVART);
        else if (dy < 0)
            rita_fyrkant(old_x, old_y + BALL_SIZE + dy, BALL_SIZE, -dy, SVART);
    }

    // Rita bollen på nya positionen
    rita_fyrkant(ball_x, ball_y, BALL_SIZE, BALL_SIZE, VITT);
}


static void update_frame(uint32_t sw) //Bruno
{
    int old_left_y = left_y;
    int old_right_y = right_y;

    // Uppdatera paddlarna
    move_paddles(sw);
    clamp_paddles();

    uppdatera_paddel(left_x, old_left_y, left_y);
    uppdatera_paddel(right_x, old_right_y, right_y);

    // Uppdatera bollen
    uppdatera_boll();
}

// Själva spelet

int main(void) //Tilsammans pusslat
{
    reset_score();
    set_displays(2, -1);
    set_displays(3, -1);
    clear_screen();
    rita_fyrkant(left_x, left_y, PADDLE_WIDTH, PADDLE_HEIGHT, VITT);
    rita_fyrkant(right_x, right_y, PADDLE_WIDTH, PADDLE_HEIGHT, VITT);
   
    init_timer();
   
    while (1) {
        uint32_t sw = read_switches();

        if (sw & (1u << 6))
            reset_score();

        if (sw & (1u << 7)) {
            delay();
            continue;
        }

        if (left_score >= WIN_SCORE || right_score >= WIN_SCORE) {
            delay();
            continue;
        }

        update_frame(sw);
        delay();
    }

    return 0;
}

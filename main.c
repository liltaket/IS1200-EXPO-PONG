/* main.c

   Based on IS1200 Lab 3 code.
*/

extern void enable_interrupt(void);

int timeoutcount = 0;

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


int main(void)
{
    labinit();

    while (1) {
        /*
         * Pong game loop will go here.
         */
    }

    return 0;
}
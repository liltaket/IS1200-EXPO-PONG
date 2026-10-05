/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING 
   Hampus */
   


/* Below functions are external and found in other files. */
extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );

static const int digits[10] = {//active low LUT
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

int mytime = 0x0;
char textstring[] = "text, more text, and even more text!";

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */
void labinit(void)
{}

/* Your code goes into main as well as any needed functions. */

void set_leds(int led_mask)
{
    volatile int *leds = (volatile int *)0x04000000;
    *leds = led_mask;
}
void count_to_with_leds(int stop)
{
  int counter = 0;
  set_leds(counter);

  while(counter < stop)
  {
    delay(40);//approx 1 sek delay
    counter++;
    set_leds(counter);
  
  }
}

void set_displays(int display_number, int value)
{
    volatile int *display =
        (volatile int *)(0x04000050 + display_number * 0x10);

    *display = digits[value];
}

int get_sw(void)
{
    volatile int *switches = (volatile int *)0x04000010;
    return *switches & 0x3FF;
}
int get_btn(void)
{
    volatile int *button = (volatile int *)0x040000d0;
    return *button & 0x1;
}

void display_time(int hours, int minutes, int seconds)
{
    set_displays(0, seconds % 10);
    set_displays(1, seconds / 10);

    set_displays(2, minutes % 10);
    set_displays(3, minutes / 10);

    set_displays(4, hours % 10);
    set_displays(5, hours / 10);
}
void clock_tick(int *hours, int *minutes, int *seconds)
{
    (*seconds)++;

    if (*seconds >= 60) {
        *seconds = 0;
        (*minutes)++;
    }

    if (*minutes >= 60) {
        *minutes = 0;
        (*hours)++;
    }

    if (*hours >= 24) {
        *hours = 0;
    }
}


int main(void)
{
    labinit();

    count_to_with_leds(15);

    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    while (1) {

        int sw = get_sw();

        // Bit 7 används för att rymma loopen 1000 0000
        if (sw & 0x80) {
            break;
        }

        if (get_btn()) {

            int selector = (sw >> 8) & 0x3;
            int value = sw & 0x3F;

            if (selector == 1) {
                seconds = value;
            }
            else if (selector == 2) {
                minutes = value;
            }
            else if (selector == 3) {
                hours = value;
            }
        }

        display_time(hours, minutes, seconds);

        delay(40);
        clock_tick(&hours, &minutes, &seconds);
    }

    return 0;

}



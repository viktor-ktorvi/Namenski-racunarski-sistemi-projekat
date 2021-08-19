#include <msp430.h> 
#include <stdio.h>
#include <math.h>
#include "lcd.h"
#include "utils.h"

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    LCD_init_ports();
    LCD_init();
    LCD_clear();
    LCD_cursor(1);

    char text[MAX_TEXT_LEN + 1];

    LCD_write_string("V. Todosijevic");
    LCD_cursor(2);
    LCD_write_string("Projekat br. 9");

    init_LEDs();
    init_buttons();

    init_debounce_timer();

    init_ADC();
    init_adc_timer();

    __enable_interrupt();

    while (1)
    {
        if (display_flag)
        {
            display_flag = 0;
            LCD_clear();
            LCD_cursor(1);

            sprintf(text, "Min=%d Max=%d", minimum, maximum);
            LCD_write_string(text);

            int whole, decimal;
            double mean;

            mean = (double) sum / (double) NUM_SAMPLES;

            whole = trunc(mean);
            decimal = (int) round((mean - whole) * pow(10, NUM_DIGITS));

            sprintf(text, "Mean = %d.%d", whole, decimal);

            LCD_cursor(2);
            LCD_write_string(text);
        }
    }

    return 0;
}


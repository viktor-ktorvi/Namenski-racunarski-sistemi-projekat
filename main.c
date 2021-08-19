#include <msp430.h> 
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
            display_stats();
        }
    }

    return 0;
}


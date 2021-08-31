/**
 * @file main.c
 * @brief Acquisition of voltage signal and stat display
 *
 * The program starts sampling from an analog input channel at a press of the start conversion
 * button (first button on the left). An LED bellow the button lights up and stays on until the
 * sampling is done. From that moment the input is sampled with a frequency of 10 Hz until 100
 * samples have been gathered at which point the LED turns off. Upon the press of the display
 * button (second button on the left) the maximum, minimum and mean value of the previously sampled
 * signal are displayed on the LCD. The second LED on the left turns on as well for acknowledgment
 * purposes and isn't a part of the project specifications.
 *
 *
 * @date 2021
 * @author Viktor Todosijevic (tv170050d@student.etf.bg.ac.rs)
 */
#include <msp430.h> 
#include "lcd.h"
#include "utils.h"

/**
 *  @brief main
 *
 *  Initialize LCD, LEDs, buttons, ADC and timers. Write the name of the student and the project number.
 *  In an infinite loop execute delayed interrupt processing by displaying stats on the LCD.
 */
int main(void) {
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

	while (1) {
		if (display_flag) {
			display_flag = 0;
			display_stats();
		}
	}

	return 0;
}


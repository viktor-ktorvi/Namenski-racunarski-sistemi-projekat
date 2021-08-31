/**
 * @file interrupts.c
 * @brief C interrupts
 *
 * PORT2 ISR and ADC12 ISR
 *
 * @date 2021
 * @author Viktor Todosijevic (tv170050d@student.etf.bg.ac.rs)
 */

#include "msp430.h"
#include "utils.h"

/**
 * @brief PORT2 ISR
 *
 *  Identify which button was pressed and debounce it
 */
void __attribute__ ((interrupt(PORT2_VECTOR))) P2ISR(void) {

	// assert which button was pressed

	if (P2IFG & start_button) {
		pressed_button = start_button;
	} else if (P2IFG & display_button) {
		pressed_button = display_button;
	}

	debounce_button(pressed_button); //  start debounce timer, clear interrput flag on that button, disable new interrupts on that button

}

/**
 * @brief ADC12 ISR
 *
 * Saves the result of the ADC and calculate the stats
 *
 */
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR(void) {
	//  if conversion is finished on ADC12MEM0 save results

	if (ADC12IFG & ADC12IFG0) {
		data_array[counter++] = (ADC12MEM0 >> 8) & 0x0f;     //  upper 8 bits

		if (counter == NUM_SAMPLES) {
			counter = 0;
			P4OUT &= ~indicator_LED;

			TA1CTL &= ~(MC0 | MC1);     //  stop and clear ADC timer
			TA1CTL |= TACLR;

			minimum = data_array[0];
			maximum = data_array[0];
			sum = 0;

			int i;
			for (i = 0; i < NUM_SAMPLES; i++) {
				sum += data_array[i];

				if (data_array[i] < minimum)
					minimum = data_array[i];

				if (data_array[i] > maximum)
					maximum = data_array[i];
			}
		}
	}
}

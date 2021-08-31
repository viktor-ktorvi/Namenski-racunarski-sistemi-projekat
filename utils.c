/**
 * @file utils.c
 * @brief General helper function and variable definitions
 *
 * Function definitions for initializing LEDs, buttons, timers and the ADC, as well
 * as other general functions and global variables.
 *
 * @date 2021
 * @author Viktor Todosijevic (tv170050d@student.etf.bg.ac.rs)
 */

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "msp430.h"
#include "utils.h"
#include "lcd.h"

// LEDs
const uint16_t indicator_LED = BIT3;
const uint16_t debug_LED = BIT4;

// buttons
const uint16_t start_button = BIT4;
const uint16_t display_button = BIT5;

// ADC
const uint16_t adc_input_channel = ADC12INCH_14;

int counter = 0;
unsigned sum = 0;
uint16_t minimum = 0;
uint16_t maximum = 0;

int display_flag = 0;

/**
 *  @brief init_LEDs
 *
 *  set the port direction of GPIOs controlling LEDs to output and clear them
 *
 */
void init_LEDs() {
	const uint16_t leds[] = { indicator_LED, debug_LED };

	int i;
	for (i = 0; i < sizeof(leds) / sizeof(leds[0]); i++) {
		P4DIR |= leds[i];   //  set to output
		P4OUT &= ~leds[i];  //  clear output
	}
}

/**
 *  @brief init_buttons
 *
 *  set the GPIO ports intended for the buttons to INPUT, falling edge triggering,
 *  clear interrupt flags and enable interrupts
 */
void init_buttons() {

	const uint16_t buttons[] = { start_button, display_button };

	int i;
	for (i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
		P2DIR &= ~buttons[i];    //  clear for in
		P2IES |= buttons[i];     //  falling edge
		P2IFG &= ~buttons[i];    //  clear interrupt flag
		P2IE |= buttons[i];      //  enable interrupt
	}
}

/**
 *  @brief init_debounce_timer
 *
 *  initialize timer A0 intended for debouncing buttons
 */
void init_debounce_timer() {
	//  initialize timer A
	TA0CCR0 = TIMER_PERIOD_A0;     //  debounce period
	TA0CCTL0 = CCIE;            //  enable CCR0 interrupt
	TA0CTL = TASSEL__ACLK;      //  clock
}

/**
 *  @brief debounce_button
 *
 *  start debounce timer, clear interrput flag on that button, disable new interrupts on that button
 */
void debounce_button(uint16_t button) {
	TA0CTL |= MC__UP;           //  start debounce timer
	P2IFG &= ~button;           //  clear button flag
	P2IE &= ~button;            //  disable button interrupts
}

/**
 *  @brief init_adc
 *
 *  initialize ADC
 */
void init_ADC() {
	ADC12CTL0 = ADC12ON | ADC12SHT0_8;  //  turn on ADC and set sampling timer
	ADC12CTL1 = ADC12SHP; //  set SHP to use SAMPCON; single channel single conversion
	ADC12MCTL0 = adc_input_channel;     //  set to chosen channel
	ADC12CTL0 |= ADC12ENC; //  enable conversion, note: to change channel disable conversion then enable it
	ADC12IE |= BIT0;                    //  enable interrupt on ADC12MEM0
}

/**
 *  @brief init_adc_timer
 *
 *  initialize timer A1 - used for periodically starting ADC
 */
void init_adc_timer() {

	TA1CCR0 = TIMER_PERIOD_A1;      //  conversion period
	TA1CCTL0 = CCIE;                //  enable CCR0 interrupt
	TA1CTL = TASSEL__ACLK;          //  clock
}

/**
 *  @brief display_stats
 *
 *  Write the samples signal stats on the LCD
 */
void display_stats() {
	char text[MAX_TEXT_LEN + 1];
	int whole, decimal;
	double mean;

	LCD_clear();
	LCD_cursor(1);

	sprintf(text, "Min=%d Max=%d", minimum, maximum);
	LCD_write_string(text);

	mean = (double) sum / (double) NUM_SAMPLES;

	whole = trunc(mean);
	decimal = (int) round((mean - whole) * pow(10, NUM_DIGITS));

	sprintf(text, "Mean=%d.%d", whole, decimal);

	LCD_cursor(2);
	LCD_write_string(text);
}

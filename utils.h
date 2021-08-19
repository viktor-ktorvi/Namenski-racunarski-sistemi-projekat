/*
 * utils.h
 *
 *  Created on: Aug 18, 2021
 *      Author: msp
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

#define TIMER_PERIOD_A0 (1023)      //  (2^10 - 1 ) / 2^15 ms = ~31ms
#define TIMER_PERIOD_A1 (3277)      //  100ms ?
#define NUM_SAMPLES (100)

#define MAX_TEXT_LEN (16)
#define NUM_DIGITS (2)

// LEDs
extern const uint16_t indicator_LED;
extern const uint16_t debug_LED;

// buttons
extern const uint16_t start_button;
extern const uint16_t display_button;
uint16_t pressed_button;

// ADC
extern const uint16_t adc_input_channel;

uint16_t data_array[NUM_SAMPLES];

extern int counter;
extern unsigned sum;
extern uint16_t minimum;
extern uint16_t maximum;

extern int display_flag;

void init_LEDs();
void init_buttons();
void init_debounce_timer();
void debounce_button(uint16_t button);
void init_ADC();
void init_adc_timer();

#endif /* UTILS_H_ */

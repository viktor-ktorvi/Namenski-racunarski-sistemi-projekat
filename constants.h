/*
 * constants.h
 *
 *  Created on: Aug 15, 2021
 *      Author: msp
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <stdint.h>
#include <msp430.h>

#define TIMER_PERIOD_A0 (1023)      //  (2^10 - 1 ) / 2^15 ms = ~31ms

// TODO Da li je dobar ovaj period. Jer kad merim vreme ispada oko 16 sec
#define TIMER_PERIOD_A1 (3277)     //  (2^15 - 1 ) / 2^15 / 10 ms = ~100ms

#define NUM_DATA_SAMPLES (100)

uint16_t pressed_button = 0;

uint16_t data = 0x0;
unsigned int ad_result = 0;
uint16_t data_array[NUM_DATA_SAMPLES];
unsigned int data_counter = 0;
double mean = 0;
int min = 0;
int max = 0;

uint16_t adc_input_channel = ADC12INCH_14;

//  button register locations
const uint16_t buttons[] = { BIT4, BIT5 };

const uint16_t debug_LED = BIT6;
const uint16_t indicator_LED = BIT3;

#endif /* CONSTANTS_H_ */

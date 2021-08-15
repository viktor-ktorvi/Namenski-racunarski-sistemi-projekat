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

#define TIMER_PERIOD (1023)     //  (2^10 - 1 ) / 2^15 ms = ~31ms


uint16_t pressed_button = 0;
uint16_t corresponding_led = 0;

//  LED register positions
const uint16_t led_bits[] = { BIT3, BIT4 };

//  button register locations
const uint16_t buttons[] = { BIT4, BIT5 };

#endif /* CONSTANTS_H_ */

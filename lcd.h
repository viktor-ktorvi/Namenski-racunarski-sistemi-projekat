/*
 * lcd.h
 *
 *  Created on: Aug 15, 2021
 *      Author: msp
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include <msp430.h>

const uint16_t register_select = BIT2;
const uint16_t enable = BIT3;

const uint16_t lcd_data[] = {BIT4, BIT5, BIT6, BIT7};

void LCD_init_ports();
void LCD_init();
void LCD_write(uint8_t data);
void LCD_write_instruction(uint8_t data);
void LCD_clear();
void LCD_cursor(int row);
void LCD_write_data(uint8_t data);
void LCD_write_char(char character);
void LCD_write_string(char* string);

#endif /* LCD_H_ */

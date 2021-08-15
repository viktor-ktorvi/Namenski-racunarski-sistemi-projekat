/*
 * lcd.c
 *
 *  Created on: Aug 15, 2021
 *      Author: msp
 */

#include "lcd.h"
char * int2str(long num);
#include <stdlib.h>
extern char *str_for_int;
/**
 *  @brief LCD_init_ports
 *
 *  set the GPIO ports intended for the LCD to OUTPUT
 */
void LCD_init_ports()
{
    P8DIR |= register_select | enable;

    P8OUT &= ~register_select;
    P8OUT &= ~enable;

    int i;
    for (i = 0; i < sizeof(lcd_data) / sizeof(lcd_data[0]); i++)
    {
        P8DIR |= lcd_data[i];   //  set all to OUTPUT
        P8OUT &= ~lcd_data[i];  //  clear all

    }

}

/**
 *  @brief LCD_init
 *
 *  initialize the LCD
 */
void LCD_init()
{

    uint16_t instructions[] = { 0x03, 0x03, 0x03, 0x02,       //  power on
                                0x02, 0x08,              //  number of lines = 2
                                0x00, 0x0C, 0x00, 0x06 }; //  i'm not sure, I got this online

    int i;
    for (i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++)
    {
        LCD_write_instruction(instructions[i]);
    }
}

/**
 *  @brief LCD_write
 *
 *  set the 4-bit data pins of the LCD regardless of RS
 */
void LCD_write(uint8_t data)
{
    uint16_t bits[] = { BIT0, BIT1, BIT2, BIT3 };

    // for all the bits in the lower nibble
    int i;
    for (i = 0; i < sizeof(bits) / sizeof(bits[0]); i++)
    {
        if (data & bits[i])
            P8OUT |= lcd_data[i];   //  if it's 1 set
        else
            P8OUT &= ~lcd_data[i];  //  if it's 0 clear
    }
}

/**
 *  @brief LCD_write_instruction
 *
 *  write data to the LCD as an instruction
 */
void LCD_write_instruction(uint8_t data)
{
    P8OUT &= ~register_select;  //  set RS to instruction mode
    LCD_write(data);            //  write to the data pins

    // enable impulse
    P8OUT |= enable;
    P8OUT &= ~enable;

}

/**
 *  @brief LCD_write_data
 *
 *  write data to the LCD as data
 */
void LCD_write_data(uint8_t data)
{
    P8OUT |= register_select;   //  set RS to data mode
    LCD_write(data);            //  write to the data pins

    // enable impulse
    P8OUT |= enable;
    P8OUT &= ~enable;

}

/**
 *  @brief LCD_clear
 *
 *  clear the LCD
 */
void LCD_clear()
{
    LCD_write_instruction(0);
    LCD_write_instruction(1);

}

/**
 *  @brief LCD_cursor
 *
 *  set the cursor to the desired row
 */
void LCD_cursor(int row)
{
    if (row == 1)
    {
        LCD_write_instruction(0x08);
        LCD_write_instruction(0x00);
    }
    else if (row == 2)
    {
        LCD_write_instruction(0x0C);
        LCD_write_instruction(0x00);
    }
}

/**
 *  @brief LCD_write_char
 *
 *  writes the HIGH then the LOW of a char
 */
void LCD_write_char(char character)
{
    LCD_write_data((character & 0xF0) >> 4);    // write HIGH of char
    LCD_write_data((character & 0x0F));         // write LOW of char
}

/**
 *  @brief LCD_write_string
 *
 *  writes a series of chars
 */
void LCD_write_string(char* string)
{
    int i;
    for (i = 0; string[i] != '\0'; i++)
        LCD_write_char(string[i]);
}

void LCD_write_integer(long num)
{
    LCD_write_string(int2str(num));

    free(str_for_int);
    str_for_int = NULL;
}


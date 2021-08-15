/**
 * @file main.c
 * @brief Projekat
 *
 * Projekat br. 9
 *
 * @date 2021
 * @author Viktor Todosijevic (tv170050d@student.etf.bg.ac.rs)
 */
#include <msp430.h> 
#include <stdint.h>

void LCD_init();
void LCD_write(uint8_t rs, uint8_t val);
void LCD_write_nibble(uint16_t nibble[], uint8_t val);

#define TIMER_PERIOD (1023)     //  (2^10 - 1 ) / 2^15 ms = ~31ms
#define CLOCK_PERIDO (1048576)	//	1s?
#define t_AS (1)				//	1us? > 40ns
#define t_DSW (1)				//	1us? > 80ns
#define t_H (1)					//	1us? > 10ns
#define t_BETWEEN_NIBBLES (1)	//	just to be sure

// TODO Osposobiti LCD i napisati nesto na njemu, bez toga ne mogu da debagujem ADC

uint16_t pressed_button = 0;
uint16_t corresponding_led = 0;
uint8_t character = 'v';


//  LED register positions
uint16_t led_bits[] = {
                       BIT3,
                       BIT4
};

//  button register locations
uint16_t buttons[] = {
                      BIT4,
                      BIT5
};

#define register_select lcd_gpios[0]
#define enable lcd_gpios[1]

uint16_t lcd_gpios[] = {
					   BIT2,	//	register select
					   BIT3,	//	enable
					   BIT4,	//	4
					   BIT5,	//	5
					   BIT6,	//	6
					   BIT7		//	7
};

uint16_t lcd_data[] = {
					   BIT4,	//	4
					   BIT5,	//	5
					   BIT6,	//	6
					   BIT7		//	7
};

void LCD_init()
{
	int i;
	for(i = 0; i < sizeof(lcd_gpios) / sizeof(lcd_gpios[0]); i++)
	{
		P8DIR |= lcd_gpios[i];		//	set the LCD intended GPIO pins for output
		P8OUT &= ~lcd_gpios[i];		//	set them to LOW
	}
}

//	https://www.sparkfun.com/datasheets/LCD/HD44780.pdf

void LCD_write_nibble(uint16_t nibble[], uint8_t val)
{
	P8OUT |= enable;	//	enable rising edge


	int i;
	for (i = 0; i < 4; i++)
	{	
		//	if val at position is 1 set, else clear
		if (val & nibble[i])
			P8OUT |= lcd_data[i];
		else
			P8OUT &= ~ lcd_data[i];
	}

	__delay_cycles(t_DSW);	

	P8OUT &= ~enable;	//	enable falling edge
}

void LCD_write(uint8_t rs, uint8_t val)
{

	if (rs == 1)						// set/clear RS depending on instruction or data
		P8OUT |= register_select;
	else if (rs == 0)
		P8OUT &= ~register_select;
	
	__delay_cycles(t_AS);				//	timing characteristics? page 58


	uint16_t upper[] = {BIT7, BIT6, BIT5, BIT4};	//	set upper 4 bits

	LCD_write_nibble(upper, val);

	__delay_cycles(t_BETWEEN_NIBBLES)


	uint16_t lower[] = {BIT3, BIT2, BIT1, BIT0};	//	set lower 4 bits

	LCD_write_nibble(lower, val);

}


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	//  stop watchdog timer

    //  2 LEDs  out
	int i;
	for (i = 0; i < sizeof(led_bits) / sizeof(led_bits[0]); i++)    //  iterate through array
	{
	    P4DIR |= led_bits[i];   //  set for out
	    P4OUT &= ~led_bits[i];  //  turn LEDs off initially
	}

	//  2 buttons in
	for (i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
	{
	    P2DIR &= ~buttons[i];    //  clear for in
	    P2IES |= buttons[i];     //  falling edge
	    P2IFG &= ~buttons[i];    //  clear interrupt flag
	    P2IE |= buttons[i];      //  enable interrupt
	}

	//  initialize timer A
	TA0CCR0 = TIMER_PERIOD;     //  debounce period
	TA0CCTL0 = CCIE;            //  enable CCR0 interrupt
	TA0CTL = TASSEL__ACLK;      //  clock

	LCD_init();
	LCD_write(1, character);


	__enable_interrupt();   //  global interrupt enable

	while(1)
	{

	}
	
	return 0;
}

/**
 * @brief PORT2 ISR
 *
 * Registers a button press and starts the debounce timer. Changes the ADC
 * input channel to the one corresponding to the pressed button.
 *
 */
void __attribute__ ((interrupt(PORT2_VECTOR))) P2ISR(void)
{
    int i;
    //  go trough all buttons and see which one is pressed
    for (i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
        if (P2IFG & buttons[i])
        {
            TA0CTL |= MC__UP;               //  start timer
            P2IFG &= ~buttons[i];           //  clear button flag
            P2IE &= ~buttons[i];            //  disable button interrupts
            pressed_button = buttons[i];    //  specify which button was pressed
			corresponding_led = led_bits[i];

        }
    return;
}

/**
 * @brief TIMERA0 CCR0 ISR
 *
 * Starts ADC if button is actually pressed
 *
 */
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) CCR0ISR (void)
{
    if ((P2IN & pressed_button) == 0)   // if button is still pressed start conversion
		P4OUT ^= corresponding_led;		// toggle correspoding led

        

    TA0CTL &= ~(MC0 | MC1);     //  stop and clear timer
    TA0CTL |= TACLR;
    P2IFG &= ~pressed_button;   //  clear button flag
    P2IE |= pressed_button;     //  enable interrupt on button

    return;
}
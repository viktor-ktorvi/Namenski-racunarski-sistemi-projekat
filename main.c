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

#define TIMER_PERIOD (1023)     //  (2^10 - 1 ) / 2^15 ms = ~31ms

// TODO Osposobiti LCD i napisati nesto na njemu, bez toga ne mogu da debagujem ADC

uint16_t pressed_button = 0;
uint16_t corresponding_led = 0;


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
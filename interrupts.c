/*
 * interrupts.c
 *
 *  Created on: Aug 18, 2021
 *      Author: msp
 */

#include "msp430.h"
#include "utils.h"

/**
 * @brief PORT2 ISR
 *
 *  identify which button was pressed and debounce it
 */
void __attribute__ ((interrupt(PORT2_VECTOR))) P2ISR(void)
{

    // assert which button was pressed

    if (P2IFG & start_button)
    {
        pressed_button = start_button;
    }
    else if (P2IFG & display_button)
    {
        pressed_button = display_button;
    }

    debounce_button(pressed_button); //  start debounce timer, clear interrput flag on that button, disable new interrupts on that button

}

/**
 * @brief TIMERA0 CCR0 ISR
 *
 *  Do something depending on the button if button is actually pressed
 *
 */
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) CCR0ISR(void)
{
    if ((P2IN & pressed_button) == 0) // if button is still pressed start conversion
    {
        if (pressed_button == start_button)
        {
            P4OUT |= indicator_LED;
            TA1CTL |= MC__UP;   //  start counting up with ADC counter
        }
        else if (pressed_button == display_button)
        {
            P4OUT ^= debug_LED;
            display_flag = 1;
        }
    }

    TA0CTL &= ~(MC0 | MC1);     //  stop and clear debounce timer
    TA0CTL |= TACLR;
    P2IFG &= ~pressed_button;     //  clear button flag
    P2IE |= pressed_button;     //  enable interrupt on button

    return;
}

/**
 * @brief TIMERA1 CCR0 ISR
 *
 * Periodically start conversion.
 *
 */
//void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) A1_CCR0ISR(void)
//{
//    ADC12CTL0 |= ADC12SC;   //  start conversion
//    return;
//}

/**
 * @brief ADC12 ISR
 *
 * Saves the result of the ADC
 *
 */
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR(void)
{
    //  if conversion is finished on ADC12MEM0 save results

    if (ADC12IFG & ADC12IFG0)
    {
        data_array[counter++] = (ADC12MEM0 >> 8) & 0x0f;     //  upper 8 bits

        if (counter == NUM_SAMPLES)
        {
            counter = 0;
            P4OUT &= ~indicator_LED;

            TA1CTL &= ~(MC0 | MC1);     //  stop and clear ADC timer
            TA1CTL |= TACLR;

            minimum = data_array[0];
            maximum = data_array[0];
            sum = 0;

            int i;
            for (i = 0; i < NUM_SAMPLES; i++)
            {
                sum += data_array[i];

                if (data_array[i] < minimum)
                    minimum = data_array[i];

                if (data_array[i] > maximum)
                    maximum = data_array[i];
            }
        }
    }
}

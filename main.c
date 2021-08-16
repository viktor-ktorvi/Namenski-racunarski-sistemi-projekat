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
#include <stdlib.h>
#include <math.h>
#include "constants.h"
//#include "lcd.h"

void init_buttons();
void init_debounce_timer();
void init_adc_timer();
void init_adc();
char * int2str(long num);
char * double2str(double num, int num_decimals);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   //  stop watchdog timer

    P4DIR |= debug_LED;
    P4OUT &= ~debug_LED;

    P4DIR |= indicator_LED;
    P4OUT &= ~indicator_LED;

    LCD_init_ports();
    LCD_init();
    LCD_clear();
    LCD_cursor(1);

//    LCD_write_string(double2str(555333.567, 2));
    LCD_write_string("V. Todosijevic");
    LCD_cursor(2);
    LCD_write_string("Projekat br. 9");

    init_buttons();
    init_debounce_timer();
    init_adc_timer();
    init_adc();

    __enable_interrupt();   //  global interrupt enable

    while (1)
    {
        if (display_flag)
        {
            display_flag = 0;
//            mean = (float)(sum / 100.0);  // sto ovo puca!?!?
            LCD_clear();
            LCD_cursor(1);
            LCD_write_string("Min = ");
            LCD_write_integer((long) min);
            LCD_write_string(" Max = ");
            LCD_write_integer((long) max);
            LCD_cursor(2);
            LCD_write_string("Mean = ");


        }
    }

    return 0;
}

/**
 *  @brief init_buttons
 *
 *  set the GPIO ports intended for the buttons to INPUT, falling edge triggering,
 *  clear interrupt flags and enable interrupts
 */
void init_buttons()
{
    //  2 buttons in
    int i;
    for (i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
    {
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
void init_debounce_timer()
{
    //  initialize timer A
    TA0CCR0 = TIMER_PERIOD_A0;     //  debounce period
    TA0CCTL0 = CCIE;            //  enable CCR0 interrupt
    TA0CTL = TASSEL__ACLK;      //  clock
}

/**
 *  @brief init_adc_timer
 *
 *  initialize timer A1 - used for periodically starting ADC
 */
void init_adc_timer()
{

    TA1CCR0 = TIMER_PERIOD_A1;      //  conversion period
    TA1CCTL0 = CCIE;                //  enable CCR0 interrupt
    TA1CTL = TASSEL__ACLK;          //  clock
}

/**
 *  @brief init_adc
 *
 *  initialize ADC
 */
void init_adc()
{
    ADC12CTL0 = ADC12ON | ADC12SHT0_8;  //  turn on ADC and set sampling timer
    ADC12CTL1 = ADC12SHP; //  set SHP to use SAMPCON; single channel single conversion
    ADC12MCTL0 = adc_input_channel;     //  set to chosen channel
    ADC12CTL0 |= ADC12ENC; //  enable conversion, note: to change channel disable conversion then enable it
    ADC12IE |= BIT0;                    //  enable interrupt on ADC12MEM0
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
        }
    return;
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
        if (pressed_button == buttons[0])
        {
            TA1CTL |= MC__UP;   //  start counting up with ADC counter
            P4OUT |= indicator_LED;
        }
        else if (pressed_button == buttons[1])
        {
            P4OUT ^= debug_LED;
            display_flag = 1;     //  delayed interrupt processing
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
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) A1_CCR0ISR(void)
{
    ADC12CTL0 |= ADC12SC;   //  start conversion
    return;
}

/**
 * @brief ADC12 ISR
 *
 * Saves the result of the ADC
 *
 */
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR(void)
{
// if conversion is finished on ADC12MEM0 save results
    if (ADC12IFG & ADC12IFG0)
    {
        ad_result = ADC12MEM0;
        data = (ADC12MEM0 >> 8) & 0x0f;     //  upper 8 bits

        data_array[data_counter++] = data;

        if (data_counter == NUM_DATA_SAMPLES)
        {
            data_counter = 0;

            TA1CTL &= ~(MC0 | MC1);     //  stop and clear ADC timer
            TA1CTL |= TACLR;

            int i;
            sum = 0;
            min = data_array[0];
            max = data_array[0];
            for (i = 0; i < NUM_DATA_SAMPLES; i++)
            {
                sum += data_array[i];

                if (data_array[i] > max)
                    max = data_array[i];

                if (data_array[i] < min)
                    min = data_array[i];

            }

            mean = sum / 100.0;

            P4OUT &= ~indicator_LED;

        }
    }
}

/**
 *  @brief int2str
 *
 *  turn integer to string
 */
char * int2str(long num)
{

    int i;
    for (i = 0; i < MAX_STR_SIZE; i++)
    {
        str_reverse_for_int[i] = '0' + (num % 10);
        num /= 10;

        if (num == 0)
        {
            str_reverse_for_int[i + 1] = '\0';

            break;
        }

    }

    str_for_int = malloc((i + 2) * sizeof(char));

    int j;
    for (j = i; j >= 0; j--)
    {
        str_for_int[j] = str_reverse_for_int[i - j];
    }

    str_for_int[i + 1] = '\0';

    return str_for_int;
}

char * double2str(double num, int num_decimals)
{
    long whole = trunc(num);
    double decimal = num - whole;

    str_whole = int2str(whole);

    int i;
    for (i = 0; i < num_decimals; i++)
        decimal *= 10;

    str_decimal = int2str((long) round(decimal));

    int len_whole = 0;
    for (i = 0; str_whole[i] != '\0'; i++)
        len_whole++;

    int len_decimal = 0;
    for (i = 0; str_decimal[i] != '\0'; i++)
        len_decimal++;

    // letters + . + \0
    str_double = malloc((len_decimal + len_whole + 2) * sizeof(char));

    int j;
    for (j = 0; j < len_whole; j++)
        str_double[j] = str_whole[j];

    str_double[len_whole] = '.';
    for (j = 0; j < len_decimal; j++)
        str_double[len_whole + 1 + j] = str_decimal[j];

    str_double[len_whole + len_decimal + 1] = '\0';

    return str_double;
}

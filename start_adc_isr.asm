		.cdecls C,LIST,"msp430.h"

		.text

A1_CCR0ISR:
		OR.W      #1,&ADC12CTL0+0
		RETI

            	.sect   .int49			; see TIMER1_A0_VECTOR macro
		.short	A1_CCR0ISR

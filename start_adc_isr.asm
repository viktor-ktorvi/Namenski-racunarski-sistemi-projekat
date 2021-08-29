		.cdecls C,LIST,"msp430.h"

		.text

A1_CCR0ISR:
		bis.w      #ADC12SC,&ADC12CTL0
		reti

	        .sect   .int49			; see TIMER1_A0_VECTOR macro
		.short	A1_CCR0ISR

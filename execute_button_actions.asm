			.cdecls C,LIST,"msp430.h"

			.ref	pressed_button
			.ref	start_button
			.ref	display_button

			.ref 	debug_LED
			.ref 	indicator_LED

			.ref	display_flag

			.text

CCR0ISR:
			bit.b 	&pressed_button,&P2IN
			jne		RESET_TIMER_AND_BUTTONS

	        cmp.w  	&start_button,&pressed_button
        	jeq		START_BUTTON_PRESSED

			cmp.w	&display_button,&pressed_button
			jeq		DISPLAY_BUTTON_PRESSED

			jmp RESET_TIMER_AND_BUTTONS


START_BUTTON_PRESSED:

			bis.b	&indicator_LED,&P4OUT
			bis.w	#MC__UP,&TA1CTL

			jmp RESET_TIMER_AND_BUTTONS

DISPLAY_BUTTON_PRESSED:

			xor.b	&debug_LED,&P4OUT
			mov.w     #1,&display_flag

			jmp RESET_TIMER_AND_BUTTONS




RESET_TIMER_AND_BUTTONS:

			bic.w 	#MC0,&TA0CTL
			bic.w 	#MC1,&TA0CTL

			bis.w	#TACLR,&TA0CTL

			bic.b	&pressed_button,&P2IFG
			bis.b	&pressed_button,&P2IE

			reti


            .sect   .int54			; see TIMER0_A0_VECTOR macro
			.short	CCR0ISR

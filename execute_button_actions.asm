; @file execute_button_actions.asm
; @brief TIMER A0 CCR0 interrupt service routine
;
; Because timer A1 is used for button debouncing and after it's period is finished
; check whether the button is still pressed, if it is, determine which and
; execute the appropriate action. If the button isn't pressed reset the button
; and timer, also do this after executing actions.
;
; @date 2021
; @author Viktor Todosijevic (tv170050d@student.etf.bg.ac.rs)

;-------------------------------------------------------------------------------
; Include device header file
			.cdecls C,LIST,"msp430.h"
;-------------------------------------------------------------------------------
; Declare external variables
			.ref	pressed_button
			.ref	start_button
			.ref	display_button

			.ref 	debug_LED
			.ref 	indicator_LED

			.ref	display_flag
;-------------------------------------------------------------------------------
; ISR
			.text

CCR0ISR:
			bit.b 	&pressed_button,&P2IN				; check if button is pressed
			jne		RESET_TIMER_AND_BUTTONS				; if not jump to RESET_TIMER_AND_BUTTONS

	        cmp.w  	&start_button,&pressed_button		; check if start_button is pressed
        	jeq		START_BUTTON_PRESSED				; if yes jump to START_BUTTON_PRESSED

			cmp.w	&display_button,&pressed_button		; check if display_button is pressed
			jeq		DISPLAY_BUTTON_PRESSED				; if yes jump to DISPLAY_BUTTON_PRESSED

			jmp 	RESET_TIMER_AND_BUTTONS				; if the button that was pressed wasn't one of the mentioned jump to RESET_TIMER_AND_BUTTONS


START_BUTTON_PRESSED:

			bis.b	&indicator_LED,&P4OUT				; set the indicator_LED
			bis.w	#MC__UP,&TA1CTL						; start TIMER A1

			jmp 	RESET_TIMER_AND_BUTTONS				; jump to RESET_TIMER_AND_BUTTONS

DISPLAY_BUTTON_PRESSED:

			xor.b	&debug_LED,&P4OUT					; toggle the debug_LED
			mov.w   #1,&display_flag					; set the display flag for delayed interrupt processing

			jmp 	RESET_TIMER_AND_BUTTONS				; jump to RESET_TIMER_AND_BUTTONS




RESET_TIMER_AND_BUTTONS:

			bic.w 	#MC0,&TA0CTL						; stop timer
			bic.w 	#MC1,&TA0CTL

			bis.w	#TACLR,&TA0CTL						; clear timer

			bic.b	&pressed_button,&P2IFG				; clear the pressed buttons interrupt flag
			bis.b	&pressed_button,&P2IE				; enable button interrupt

			reti										; return

;-------------------------------------------------------------------------------
; Interrupt vector specification
            .sect   .int54								; see TIMER0_A0_VECTOR macro
			.short	CCR0ISR
;-------------------------------------------------------------------------------


; @file start_adc_isr.asm
; @brief TIMER A1 CCR0 interrupt service routine
;
; ISR starts ADC conversion every time timer A1 finishes counting.
;
; @date 2021
; @author Viktor Todosijevic (tv170050d@student.etf.bg.ac.rs)

;-------------------------------------------------------------------------------
; Include device header file
			.cdecls C,LIST,"msp430.h"
;-------------------------------------------------------------------------------
; ISR
			.text
A1_CCR0ISR:
			bis.w      #ADC12SC,&ADC12CTL0	; Set for start of conversion
			reti							; return
;-------------------------------------------------------------------------------
; Interrupt vector specification
            .sect   .int49					; see TIMER1_A0_VECTOR macro
			.short	A1_CCR0ISR
;-------------------------------------------------------------------------------


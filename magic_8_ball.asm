;FILE NAME: magic_8_ball.asm
;AUTHOR: Nrabow
;DATE STARTED: 2/24/2020
;DESCRIPTION: Upon reset all the leds are off. The serial port is configured with
;             9600 baud with 8 bits and one stop bit. Immediately after rst a carriage return
;             (ASCII 0DH) and a line feed (ASCII 0AH) shall be sent via the serial port. Each
;             time a button is pressed or any character is received on the serial port, exactly
;             one of the 10 LEDs shall be randomly lit and a corresponding message will be sent
;             via the serial port. 
;             10 messages are: IT IS CERTAIN; YOU MAY RELY ON IT; WITHOUT A DOUBT; YES; MOST LIKELY
;                              REPLY HAZY, TRY AGAIN; CONCENTRATE AND ASK AGAIN; DON'T COUNT ON IT;
;                              VERY DOUBTFUL; MY REPLY IS NO
;             each message is terminated by a carraige return and a line feed.

$include (c8051f020.inc)
LED_PORT EQU P1
LED_8 BIT P2.0
LED_9 BIT P2.1
BTN_1 BIT P3.0

DSEG AT 30H
RANDOM: DS 1
BTN_STATUS: DS 1


CSEG AT 0
;MAIN LOOP
	mov wdtcn,#0DEh 	; disable watchdog
	mov	wdtcn,#0ADh
	mov	xbr2,#40h	; enable port output
	mov	xbr0,#04h	; enable uart 0
	mov	oscxcn,#67H	; turn on external crystal
	mov	tmod,#21H	; wait 1ms using T1 mode 2, T0 MODE 2
	mov	th1,#256-167	; 2MHz clock, 167 counts = 1ms
	setb	tr1
wait1:
	jnb	tf1,wait1
	clr	tr1		; 1ms has elapsed, stop timer
	clr	tf1
wait2:
	mov	a,oscxcn	; now wait for crystal to stabilize
	jnb	acc.7,wait2
	mov	oscicn,#8	; engage! Now using 22.1184MHz

	mov	scon0,#50H	; 8-bit, variable baud, receive enable
	mov	th1,#-6		; 9600 baud
	setb	tr1		; start baud clock

	MOV SBUF0, #0DH
WAIT_CAR:
	JNB TI, WAIT_CAR
	CLR TI
	MOV SBUF0, #0AH

LOOP:
	
;TIMER 0 RANDOM NUMBER GENERATOR
	

RELOAD:
	CLR TF0
	MOV TH0, #HIGH(-18432)    ;10 MS DELAY WITH TIMER 0
	MOV TL0, #LOW(-18432)			;16 BIT
	SETB TR0
TIMER_WAIT:
	JNB TF0, TIMER_WAIT         ;PUT THIS IN MAIN LOOP
	CLR TR0
	CLR TF0
	DJNZ RANDOM, CONTINUE
	MOV RANDOM, #10
CONTINUE:

	CALL BUTTON_CHECK ;CHECK FOR PRESSES
	JB RI, IN    					;
	JNZ READY
	JMP LOOP

IN: CLR RI
READY:
	MOV A, RANDOM
	CALL DISPLAY
	MOV A, RANDOM
	MOV DPTR, #TABLE
	CALL SEND_MESSAGE
	JMP LOOP

	


;DISPLAY
;turns off leds and turn on leds representing new values stored
;in player 1 & 2 using look up table
;inputs: plyaer 1 & 2
;outputs: none
;destroys: none
display:
			call led_off
			MOV A, RANDOM
			CALL LED_ON
			ret

led_off:
			mov led_port, #0ffh
			setb led_8
			setb led_9
			ret

led_on:
			
			jb acc.3, ex_led8				;jumps if led 8 or 9 need to be turned on
			add a, #LUT-PC1					;stores the difference in address between lut and pc1
			movc a, @a+pc						;moves the first address of the lut in acc
pc1:	anl p1, a								;sees which light needs to be lit
			ret
lut: db 0FEH, 0FDH, 0FBH, 0F7H, 0EFH, 0DFH, 0BFH, 07FH
	
ex_led8:jb acc.0, ex_led9
				clr p2.0
				ret
ex_led9:clr p2.1
				ret
					
;BUTTON_CHECK
button_check:
			MOV A, #0FFH
			MOV C, BTN_1
			MOV ACC.1, C			

			cpl a
			xch a, btn_status
			xrl a, btn_status
			anl a, btn_status
			ret

;SEND CHARACTER
SEND_CH:
	MOV SBUF0, A
WAIT:
	JNB TI, WAIT
	CLR TI
	RET
	
;SEND MESSAGE
SEND_MESSAGE:
	CLR A
	MOVC A, @ A+DPTR
	JZ DONE
	CALL SEND_CH
	INC DPTR
	JMP SEND_MESSAGE
DONE: 
	RET

TABLE: DB  MES_2-MES_1,MES_3-MES_1,MES_4-MES_1,MES_5-MES_1,MES_6-MES_1,MES_7-MES_1,MES_8-MES_1,MES_9-MES_1,MES_10-MES_1

MES_1: DB "It is certain", 0
MES_2: DB "You may rely on it", 0
MES_3: DB "Without a doubt", 0
MES_4: DB "Yes",  0DH, 0AH, 0
MES_5: DB "Most likely", 0
MES_6: DB "Reply hazy, try again", 0
MES_7: DB "Concentrate and ask again", 0
MES_8: DB "Don't count on it", 0
MES_9: DB "Very doubtful", 0
MES_10:DB "My reply is no", 0

	end
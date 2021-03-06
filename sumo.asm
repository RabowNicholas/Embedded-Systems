;======================================================================================
;FILE NAME: sumo.asm
;AUTHOR: nrabow
;DATE STARTED: 2-10-2020
;DESCRIPTION: 2-player game that uses bar graph and two push buttons. Each player will
;             control one of two sumo wrestlers and each will try to push the other out
;             of the ring. A DIP switch (1b-7b) will determine the starting positions. From here
;             1 second will elapse and then the LEDs will seperate, leaving two exmpty 
;             spaces. The players will press the buttons. Pushing it will move the player
;             one space closer to victory. Releasing it moves another space in the
;             desired direction. However if the other player presses before the button is
;             released, the players essentially return to start state. Once the players
;             right next to each other the next sequence begins. After a random time delay
;             they jump apart again and the same process described before happens. The game
;             is over once one of the players pushes the other off the bar graph.
;REVISION HISTORY:
; DATE			AUTHOR			DESCRIPTION
; ----      ------      -----------
; 2-20-2020	NRABOW			version 1

;HEADER FILES
$include (c8051f020.inc)

;EQAUTE STATEMENTS
led_port equ P1
led_8 bit P2.0
led_9 bit P2.1
btn1_p  bit acc.0
btn2_p  bit acc.1
btn1_r  bit b.0
btn2_r  bit b.1
switch_port equ P3
rand_number equ r7

;DATA SEGMENT
dseg at 30h
player_1: ds 1
player_2: ds 1
btn_status: ds 1

;CODE SEGMENT
cseg at 0


;MAIN LOOP
;this loop follows a flow chart I created that is stored in my notes.
;inputs: none
;outputs: none
;destroys; none
			mov wdtcn, #0DEh
			mov wdtcn, #0ADh
			mov xbr2,#40h

			call intialize

loop: call push_apart

none:;continually loops until a button is pressed
			call button_check

			jz none              ;if either button is pressed the program will continue

			jb btn1_p, pressed_1 ;pressed_1 is the path that 

pressed_2:                 ;if btn1 was not pressed that means that btn2 was pressed
			inc player_2
			call display
			;call delay
d1:		call button_check		 ;program will loop here until either btn1 is  pressed or
			jb btn1_p, move_1		 ;btn2 is released
			jb btn2_r, release_2
			jmp d1			

pressed_1:
			dec player_1
			call display
			
d0:		call button_check		 ;program will loop here until either btn2 is pressed or
			jb btn2_p, move_2		 ;btn1 is released
			jb btn1_r, release_1
			jmp d0 

;RELEASE_1
;moves player 1 to the right and displays it, then calls player1_win to check if the game is over
;inputs: player_1
;outputs: none
;destroys: none
release_1:
			dec player_1
			call display
			jmp player1_win	
;RELEASE_2
;moves player 2 to the left and displays it, then calls player2_win to check if the game is over
;inputs: player_2
;outputs: none
;destroys: none			
release_2:
			inc player_2
			call display
			jmp player2_win			

move_1:
			dec player_1
			call display
			jmp loop

move_2:
			inc player_2
			call display
			jmp loop

;GAME OVER CHECKS
;player1_win and player2_win check if the game is over and ends game by comparing the current values of
;either player 1 or player 2 and seeing if they equal 1 or 8 respectively.
;inputs: player_1, player_2
;outputs: none
;destroys: none
player1_win:
		mov a, player_1
		cjne a, #1, loop    ;if player 1 is at LED 1 the game is over
die:jmp die							;infinite loop
player2_win:
		mov a, player_2
		cjne a, #8, loop	  ;if player 2 is at LED 8 the game is over
		jmp die
					 






;INTIALIZE
;Subroutine called at the beginning of the program
;initializes data segment, stores switch input
;and uses input as handicap then displayed
;INPUTS: switch_port
;OUTPUTS: player_1 & player_2
;DESTROYS: none
intialize:
			;set all inputs
			mov P3, #0FFh
			;intialize r7
			mov rand_number, #50   ;50 is used because the delay is between .5 s and 1 s
			;intialize data segment
			mov btn_status, #0
			mov player_1, #0
			mov player_2, #0

			;set up handicap input
			clr a
			mov a, switch_port ;store switchport in acc
			cpl a						   ;only important bits are stored
			anl a, #1Ch 		   ;everything else is cleared
			rr 	a					  	 ;rotate twice so that acc equals value of input on switches
			rr  a

			jnz valid_input    ;checks for 0 on switches
			mov player_1, #5   ;if zero, set to eqaul playing field (middle)
			mov player_2, #4
			jmp continue
valid_input:
			mov player_2, a    ;store correct values in player_1 & 2 depeding upon status
			inc a							 ;of switches
			mov player_1, a
continue:
			call display
			ret
			

					 
;PUSH APART
;subroutine pushes apart the LEDs
;INPUTS: player 1 & 2, R7
;OUTPUTS: player 1 & 2
;DESTROYS: none
push_apart:
		mov a, rand_number
		add a, #50          ;50 is added to acheive a random number between 50 and 100
		mov rand_number, a
delay1:
		call delay					;RANDOM DELAY. 
		djnz r7, delay1		  ;this delay is called a random number of times.
												;that number is stored in R7
		mov a, p3           ;stores current button state from Port 3
		orl a, #0fch        ;clears everything in the acc except the button input
		cpl a
		mov btn_status, a
		inc player_1
		dec player_2
		call display
		ret	


;DISPLAY
;turns off leds and turn on leds representing new values stored
;in player 1 & 2 using look up table
;inputs: plyaer 1 & 2
;outputs: none
;destroys: none
display:
			call led_off
			mov a, player_1
			call led_on
			mov a, player_2
			call led_on
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
			


;DELAY
;creates delay without using timers.
;it is 10 ms. this is used to sample the buttons(debounce)
;INPUTS: none
;OUTPUTS: none
;DESTROYS: none
delay:															
       mov r2, #133			
place1:mov r3, #50
place2:djnz r3, place2
			 djnz r2, place1
			 ret


			

;CHECK BUTTONS
;monitors the port with buttons (p3) an checks if any button has
;been pressed or released since it was last called
;INPUTS: rand_number(R7)
;OUTPUTS: rand_number
;DESTROYS: none
button_check:
	
  djnz rand_number,carry_on
	mov rand_number,#50
carry_on:
	call delay					;10 ms to debounce buttons
	mov a, p3           ;stores current button state from Port 3
	orl a, #0fch        ;clears everything in the acc except the button input
	mov b, a            
	cpl a
	xch a, btn_status
	xrl a, btn_status    ;stores differences
	xch a, b
	anl a, b             ;stores and returns with release bits in B
	xch a, b
	anl a, btn_status    ;stores and returns with pressed bits in ACC
	ret


end

		
			

 
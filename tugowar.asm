$include (c8051f020.inc)

leds equ p1
led_8 bit p2.0
led_9 bit p2.1
btn1 bit acc.1
btn2 bit acc.0

dseg at 30h
player_1: ds 1
player_2: ds 1
btn_status: ds 1

cseg at 0

			mov wdtcn, #0DEh
			mov wdtcn, #0ADh
			mov xbr2,#40h
			call intialize
			call display
loop: call delay
			call button_check
			jb btn1, btn1_press
			jnb btn2, loop
			call move_left
			call display
			mov a, player_1
 		  cjne a, #9, loop
die:  jmp die
btn1_press:
      jb btn2, loop
			call move_right
			call display
			mov a, player_2
			cjne a, #0, loop
			jmp die
			ret
			
intialize:
			setb btn1
			setb btn2 ;set btn 1 and btn 2 as input

			setb led_8
			setb led_9

			mov leds, #11001110b
			mov player_1, #05h
			mov player_2, #04h
			mov btn_status, #0h
			
			ret
delay:
		         mov r2, #100
			there: mov r3, #133
			here:  djnz r3, here
						 djnz r2, there
						 ret


display:
			call led_off
			mov a, player_1
			call led_on
			mov a, player_2
			call led_on
			ret

led_off:
			mov leds, #0ffh
			setb led_8
			setb led_9
			ret

led_on:
			
			jb acc.3, ex_led8
			push dpl
			push dph
			anl a, #0fh
			mov dptr, #lut
			movc a, @a+dptr
			pop dph
			pop dpl
			ret
lut: db 0FEh, 0FDh, 0FBh,0F7h, 0EFh, 0DFh, 0BFh, 07Fh
ex_led8:jb acc.0, ex_led9
				clr p2.0
				ret
ex_led9:clr p2.1
				ret			
			

button_check:
			mov a,p3
			cpl a
			xch a, btn_status
			xrl a, btn_status
			anl a, btn_status
			ret

move_left:
			inc player_1
			inc player_2
			ret
		
move_right:
			dec player_1
			dec player_2
			ret
		
end				
//@author: NRabow
//Description: Doorbell with custom sounds. D/A  send signal to speaker thatplays certain notes for a determined duration
//             A button(doorbell) starts the sound.
#include <C8051F020.h>

code unsigned char sine[] = { 48, 89, 116, 126, 116, 89, 48, 0, -48, -89, -116, -126, -116, -89, -48, 0 };

unsigned char phase = sizeof(sine)-1;	// current point in sine to output

unsigned int duration = 0;		// number of cycles left to output

unsigned int envelope = 32767;

unsigned int decay = 0;


bit no_noise = 0;


void timer4(void) interrupt 16
{
	T4CON &= 0x7F;;
	DAC0H = (sine[phase] * (envelope>>8) >> 7) + 0x80;
	if ( no_noise )
	{
		duration--;
		if (duration == 0)
		{
			no_noise = 0;
		}	
	}
	else
	{
		if ( phase < sizeof(sine)-1 )	// if mid-cycle
		{				// complete it
			phase++;
		}
		else if ( duration > 0 )	// if more cycles left to go
		{				// start a new cycle
		    envelope = 32767;
			decay = 20;
			phase = 0;
			duration--;
			if (envelope > decay)
			{
				envelope -= decay;
			}
		}
	}
}
sbit BTN1 = P3^7;
sbit BTN2 = P3^6;

void main(void)
{
 	WDTCN=0x0DE; 	// disable watchdog
	WDTCN=0x0AD;
	XBR2=0x40;	// enable port output
	OSCXCN=0x67;	// turn on external crystal
	TMOD=0x20;	// wait 1ms using T1 mode 2
	TH1=256-167;	// 2MHz clock, 167 counts = 1ms
	TR1 = 1;
	while ( TF1 == 0 ) { }
	while ( (OSCXCN & 0x80) == 0 ) { }
	OSCICN=0x8;	// engage! Now using 22.1184MHz
	T4CON = 4;	// timer 2, auto reload
	RCAP4H = 0xFF;
	RCAP4L = 0x70;	// set up for 800Hz initially
	REF0CN = 3;	// turn on voltage reference
	DAC0CN = 0x94; 	// update on timer 2, left justified
	IE = 0x80;	// enable timer 2 only
	CKCON = 0x40;
	EIE2 = 6;

	for ( ; ; )
	{
		if (!BTN1)
		{
			
			RCAP4H = 0xF7; //^e 659 Hz
			RCAP4L = 0xC8;
			duration = 170;
			while (duration > 0) {}

			no_noise = 1;
			duration = 110;
			while (duration > 0) {}


			RCAP4H = 0xF7; //^e 659 Hz
			RCAP4L = 0xC8;
			duration = 170;
			while (duration > 0) {}

			no_noise = 1;
			duration = 50;
			while (duration > 0) {}

			RCAP4H = 0xF7; //^e 659 Hz
			RCAP4L = 0xC8;
			duration = 170;	
			while (duration > 0) {}

			no_noise = 1;
			duration = 50;
			while (duration > 0) {}


			RCAP2H = 0xF5; //^c 523 Hz
			RCAP2L = 0xA5;
			duration = 175;
			while (duration > 0) {}
	
			RCAP2H = 0xF7; //^e 659 Hz
			RCAP2L = 0xC8;
			duration = 220;
			while (duration > 0) {}
	
			RCAP2H = 0xF9; //^g 784 Hz
			RCAP2L = 0x17;
			duration = 600;
			while (duration > 0) {}
	
			RCAP2H = 0xF2; //g 392 Hz
			RCAP2L = 0x2F;
			duration = 196;
			while (duration > 0) {}
		}
		if (!BTN2)
		{
			RCAP2H = 0xF3; //a 440 Hz
			RCAP2L = 0xB1;
			duration = 220;
			while (duration > 0) {}

			no_noise = 1;
			duration = 110;
			while (duration > 0) {}

			RCAP2H = 0xF3; //a 440 Hz
			RCAP2L = 0xB1;
			duration = 220;
			while (duration > 0) {}

			no_noise = 1;
			duration = 110;
			while (duration > 0) {}

			RCAP2H = 0xF3; //a 440 Hz
			RCAP2L = 0xB1;
			duration = 220;
			while (duration > 0) {}

			no_noise = 1;
			duration = 110;
			while (duration > 0) {}

			RCAP2H = 0xEF; //f 330 Hz
			RCAP2L = 0x96;
			duration = 100;
			while (duration > 0) {}

			RCAP2H = 0xF5; //C 523 Hz
			RCAP2L = 0xA5;
			duration = 131;
			while (duration > 0) {}

			RCAP2H = 0xF3; //a 440 Hz
			RCAP2L = 0xB1;
			duration = 220;
			while (duration > 0) {}

			RCAP2H = 0xEF; //f 330 Hz
			RCAP2L = 0x96;
			duration = 100;
			while (duration > 0) {}

			RCAP2H = 0xF5; //C 523 Hz
			RCAP2L = 0xA5;
			duration = 131;
			while (duration > 0) {}

			RCAP2H = 0xF3; //a 440 Hz
			RCAP2L = 0xB1;
			duration = 220;
			while (duration > 0) {}




		}
	}
}

//a = 440  n=0
//a# = 466 n=1
//b = 494  n=2
//c = 523  n=3
//c# = 554 n=4
//d = 587  n=5
//d# = 622 n=6
//e = 659  n=7
//f = 699  n=8
//f# = 740 n=9
//g =784   n=10
//g# = 831 n=11
//^a = 880 n=12
//
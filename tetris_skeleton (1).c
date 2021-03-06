//@author: NRabow
//Description: Tetris/Flappy Bird like game
//	           A tetris block starts falling on the screen. Its orientation and x-axis posistion
//             are controlled by two pots. there is a button that increases fall speed.
//             A hole two blocks wide opens on the bottom of the screen and the block must be 
//             positioned/oreinted correctly to pass through the hole. Different sounds are made for
//             winning/losing. The score is display on the screen. The game ends when a player fails to get
//             block through the hole. 

#include <C8051F020.h>
#include "lcd.h"
#include "portmap.h"

#define SPEED 16128 // 35ms per move


//VARIABLES
bit collide;
char square_y;

bit start = 0;
bit score_plus = 0;
bit game_over = 0;
unsigned char score_val = 0;
unsigned char count = 0;
unsigned char rot_conv = 1;
unsigned long rotation = 0;
unsigned long horizontal = 0;
unsigned char rot_val, hor_val;
unsigned block;
unsigned char random_hole = 0;
unsigned char random_block = 0;
unsigned char random;
bit flag1 = 0;
bit flag2 = 0;
bit flag3 = 0;
bit flag4 = 0;
bit theFlag = 0;
code unsigned char sine[] = { 48, 89, 116, 126, 116, 89, 48, 0, -48, -89, -116, -126, -116, -89, -48, 0 };
unsigned char phase = sizeof(sine)-1;
unsigned int duration = 0;
unsigned int envelope = 32767;
unsigned int decay = 0;
code unsigned char mask[] = {0xf,0xf0,0x9,0x90,0x9,0x90,0xf,0xf0};


//DRAW FUNCTIONS
void draw_square(char col, char y)
{
  int i; char j;
  if ( y < 0 ) return;
  i = (col>>1) * 128 + y;
  for ( j = col & 1; j < 8; j+=2 )
  {
     if ( screen[i] & mask[j] ) collide = 1;
	 screen[i] |= mask[j];
	 i++;
  }
  	if (y == 11 & collide == 1) game_over = 1;  
}

void draw_play_area()
{
   char i;
   for (i=0; i<84; i+=4 )
   {
     draw_square(2,i+8);
	 draw_square(13,i+8);
   }
	   if (!flag1)
	   {
	   for (i = 3; i<13; i++ )
	   {
	     draw_square(i,8);
	   }
   }
   else
   {
   		for (i = 3; i<13; i++ )
	   {
	   	 if ( !(i == random_hole + 3 || i == random_hole + 4) )
	     draw_square(i,8);
	   }
   	/*switch(random_hole)
	{
		case 0:
		   for (i = 5; i<13; i++ )
		   {
		     draw_square(i,8);
		   }
			break;
		case 1:
		   for (i = 3; i<13; i++ )
		   {
			 if ( i == 5 ) i = 7;
		     draw_square(i,8);
		   }
			break;
		case 2:
			for (i = 3; i<13; i++ )
		   {
			 if ( i == 7 ) i = 9;
		     draw_square(i,8);
		   }
			break;
		case 3:
			for (i = 3; i<13; i++ )
		   {
			 if ( i == 9 ) i = 11;
		     draw_square(i,8);
		   }
			break;
		case 4:
			for (i = 3; i<13; i++ )
		   {
			 if ( i == 11 ) i = 13;
		     draw_square(i,8);
		   }
			break;
		case 5:
		}
	}*/

}


}

//LCD DISPLAY FUNCTIONS
void disp_ch(unsigned char page, unsigned char x, unsigned char ch)
{
	int ifont, iscr;
	unsigned char k;
	if (ch < ' ')
		return;

	ifont = (ch - ' ')*5;
	iscr = page*128 +x;
	for (k=0; k < 5; ++k)
	{
		screen[iscr + k] = font5x8[ifont+k];
	}

}


void display_score(void)
{
	unsigned char score_a, score_b;
	score_a = (score_val / 10) + 0x30;
	score_b = (score_val % 10) + 0x30;  	
	disp_ch(7, 100, score_a);
	disp_ch(7, 105, score_b);	
}
	


//INTERRUPT SERVICE ROUTINES
void adc0isr(void) interrupt 15
{
	if(rot_conv)
	{
		rotation += (ADC0H);
		//rotation += ADC0L;
		count++;
		if ( count == 4 )
		{
			rot_val = rotation >> 6;
			
			hor_val = horizontal / 0x55;

			rotation = 0;
			horizontal = 0;
			flag4 = 1;
			count = 0;
		}
		rot_conv = 0;
		AMX0SL = 1;
	}
	else
	{
		horizontal += (ADC0H);
		//horizontal += ADC0L;
		rot_conv = 1;
		AMX0SL = 0;

	}

	random = random << 1;
	if (ADC0L & 0x10)
		random |= 1;

	AD0INT = 0;

}

void timer4(void) interrupt 16
{
	T4CON &= 0x7F;

	DAC0H = (sine[phase] * (envelope >> 8) >> 7) + 0x80;
	if ( phase < sizeof(sine) -1)	// if mid-cycle
	{				// complete it
		phase++;
	}
	else if ( duration > 0 )	// if more cycles left to go
	{				// start a new cycle
		phase = 0;
		decay = 37;

		duration--;
		if(envelope > decay)	envelope = envelope - decay;
		if ( duration == 0 )    theFlag = 0;
	}
}

// This array defines the position of each square in each tetris block.
// each nybble of the 16-bit unsigned number represents a row from the
// 4x4 mask, the least significant nybble is the bottom row. The center
// of rotation is the second column from the left and the second row
// from the bottom.
code unsigned int bit_array[] =
{ 0x04c8, 0x00c6, 0x0264, 0x0c60,
  0x08c4, 0x006c, 0x0462, 0x06c0,
  0x00e8, 0x0446, 0x02e0, 0x0c44,
  0x00e2, 0x0644, 0x08e0, 0x044c,
  0x00e4, 0x0464, 0x04e0, 0x04c4,
  0x4444, 0x00f0, 0x4444, 0x00f0,
  0x0660, 0x0660, 0x0660, 0x0660,
};

// This array is indexed identically to bit_array but it contains the number
// of pad (unpopulated) columns on the left and right of the block. For example
// when the 4-inline block (block 5) is vertical, this arry yields 0x12, indi-
// catig one column of blank columns on the left and two such columns on the
// right
code unsigned char pad_array[] =
{ 0x02, 0x01, 0x11, 0x01,
  0x02, 0x01, 0x11, 0x01,
  0x01, 0x11, 0x01, 0x02,
  0x01, 0x11, 0x01, 0x02,
  0x01, 0x11, 0x01, 0x02,
  0x12, 0x00, 0x12, 0x00,
  0x11, 0x11, 0x11, 0x11,
};

#define LEFT_WALL   2 // left border column
#define RIGHT_WALL 13 // right border column

// function draw_block()
// Draw a 4-square tetris block. If the orientation of the block would
// cause it to intersect with the wall, its position is adjusted inward
// to avoid that. The block number is in the range 0-6. Larger numbers
// (e.g. 7) may be used to turn off the block so it is not drawn.
//
// parameters:
//   block_num: block number that denotes the configuration of the squares.
//   pos: the desired column of the block (in squares, using screen coordinates)
//   ori: orintation of the block in 90 degree increments
//   y: the y position of the block bottom in pixels (the lcd connector is +y)

void draw_block( char block_num, char pos,char ori, char y)
{
  char col, lim, j;
  char i = block_num * 4 + (ori & 3);
  unsigned int bits = bit_array[i];

  if ( block_num > 6 ) return;

  col = pos - 1; // col is the left side of the 4x4 (pos is center column)
  lim = (LEFT_WALL+1) - (pad_array[i]>>4);
  if (col < lim) col = lim;
  lim = (RIGHT_WALL-4) + (pad_array[i] & 0xf);
  if (col > lim) col = lim;
  for (i = 0; i<4; ++i)
  {
    for ( j = 3; j>=0; --j )
    {
      if ( bits & 1 ) draw_square(col+j,y+4*i);
      bits >>= 1;
    }
  }
}
//MAIN LOOP
void main()
{
   WDTCN = 0xde;  // disable watchdog
   WDTCN = 0xad;
   XBR2 = 0x40;   // enable port output
   XBR0 = 4;      // enable uart 0
   OSCXCN = 0x67; // turn on external crystal
   TMOD = 0x20;   // wait 1ms using T1 mode 2
   TH1 = -167;    // 2MHz clock, 167 counts - 1ms
   TR1 = 1;
   while ( TF1 == 0 ) { }          // wait 1ms
   while ( !(OSCXCN & 0x80) ) { }  // wait till oscillator stable
   OSCICN = 8;    // switch over to 22.1184MHz
   init_lcd();
   init_portmap();
   IE = 0x80; //interrupt enable
   IP = 0x20;
   T2CON = 4; // start timer
   RCAP2H = -SPEED>>8;
   RCAP2L = -SPEED;
   square_y = 88;

   //INITIALIZE
   //ADC
    EIE2 = 6;
	REF0CN = 3; 
    ADC0CN = 0x8D; 
	AMX0CF = 0x00;
	AMX0SL = 0x00;
   //timer4 for sounds
	T4CON = 4;//00000100
	RCAP4H = -1444>>8;
	RCAP4L = -1444;
	DAC0CN = 0x94;
	CKCON = 0x40;


	blank_screen();
	draw_play_area();
	display_score();
	refresh_screen();
	flag1 = 1;
   for ( ; ; )
   {
   			
 	
		while (!(~get_buttons() & 0x01))
		{
				random_block = random % 7;
				random_hole = random % 9;

		}

		score_val = 0;
		game_over = 0;
		square_y = 88;
	   	while (!game_over)
		{

			 if (flag4) 
			 {
			 	blank_screen();
			 			 // handle movement here
			 if ( square_y == 0 )
			 {
			   square_y = 88;
			   score_plus = 1;
			}
		     else
			   square_y -= 1;
				flag4 = 0;
				draw_play_area();
				collide = 0;
				draw_block(random_block, hor_val, rot_val, square_y);
			 

		     // draw stuff here


			 //handles score
			 if (score_plus) 
			 {
			 	score_val++;
			 	score_plus = 0;
				random_block = random % 7;
				random_hole = random % 9;


				theFlag = 1;
				RCAP4H = -700 >> 8;
				RCAP4L =-700;
				duration = 494;
				envelope = 32767;
				while (theFlag) {}

				theFlag = 1;
				RCAP4H = -526 >> 8;
				RCAP4L =-526;
				duration = 220;
				envelope = 1320;
				while (theFlag) {}


			 }
			 }
		 
			 while ( TF2 == 0 ) {}
			  	TF2 = 0;
				display_score();
				refresh_screen();

			 // turn on an LED if there is a collision
			 clear_leds();
			 if ( collide )
			 {
			 	set_leds(1);
			 }
		 }
		//game over


		//d 587 Hz
		RCAP4H = -2362 >> 8;
		RCAP4L = -2362;
		duration = 147;
		envelope = 32767;
		while (duration > 0) {}
		//f 699 Hz
		RCAP4H = -1983 >> 8;
		RCAP4L = -1983;
		duration = 175;
		envelope = 32767;
		while (duration > 0) {}
		//c 523 Hz
		RCAP4H = -2651 >> 8;
		RCAP4L = -2651;
		duration = 131;
		envelope = 32767;
		while (duration > 0) {}
		//e 659 Hz
		RCAP4H = -2104 >> 8;
		RCAP4L = -2104;
		duration = 165;
		envelope = 32767;
		while (duration > 0) {}
		//g 78S Hz
		RCAP4H = -1768 >> 8;
		RCAP4L = -1768;
		duration = 196;
		envelope = 32767;
		while (duration > 0) {}
		//g^
		RCAP4H = -886 >> 8;
		RCAP4L = -886;
		duration = 391;
		envelope = 32767;
		while (duration > 0) {}
		//a^
		RCAP4H = -788 >> 8;
		RCAP4L = -788;
		duration = 440;
		envelope = 32767;
		while (duration > 0) {}
		//d^
		RCAP4H = -589 >> 8;
		RCAP4L = -589;
		duration = 589;
		envelope = 32767;
		while (duration > 0) {}
		//g^^
		RCAP4H = -442 >> 8;
		RCAP4L =-442;
		duration = 782;
		envelope = 32767;
		while (duration > 0) {}


		
	 }

   
}


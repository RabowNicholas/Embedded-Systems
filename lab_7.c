//@Author: NRabow
//Description: Thermostat using built-in temp sensor on 8051
//			   and LCD screen. The desired temp is set using 
//             a pot and an a/d converter. Building block for
//             controller of full thermostat system.



#include <lcd.h>
#include <C8051f020.h>

int high;
bit temp_conv = 0;
unsigned long temp = 0;
unsigned long pot = 0;
long tempval, potval;
unsigned char count = 0;
bit flag = 0;
#define LED P1
sbit led8 = P2^0;
sbit led9 = P2^1;

void init_adc0(void)
{
    ADC0CN = 0x8C; 
	REF0CN = 0x07;
	AMX0CF = 0x00;
	AMX0SL = 0x00;
	EIE2 = 0x02;

}

void adc0isr(void) interrupt 15
{
	if(temp_conv)
	{
		temp += (ADC0H<<8);
		temp += ADC0L;
		count++;
		if(count == 0)
		{
			tempval = temp >>8;
			potval = pot >>8;
			pot = 0;
			temp = 0;
			flag = 1;
		}
		temp_conv = 0;
		ADC0CF = 0;
		AMX0SL = 0x00;

	}
	else
	{
		pot += (ADC0H << 8);
		pot += ADC0L;
		temp_conv = 1;
		AMX0SL = 0x08;
		ADC0CF = 1;
	}
	AD0INT = 0;
}

void timer2(void) interrupt 5
{
	TF2 = 0;
}

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

void display(void)
{
	unsigned char temp_a, temp_b, pot_a, pot_b;
	pot_a = (potval / 10) + 0x30;
	pot_b = (potval % 10) + 0x30;
	temp_a = (tempval / 10) + 0x30;
	temp_b = (tempval % 10) + 0x30;

	disp_ch(1, 0, 'S');
	disp_ch(1, 5, 'e');
	disp_ch(1, 10, 't');
	disp_ch(1, 15, ':');

	disp_ch(1, 40, pot_a);
	disp_ch(1, 45, pot_b);
	disp_ch(1, 50, 0x7F);
	disp_ch(1, 55, 'F');

	disp_ch(2, 0, 'A');
	disp_ch(2, 5, 'c');
	disp_ch(2, 10, 't');
	disp_ch(2, 15, 'u');
	disp_ch(2, 20, 'a');
	disp_ch(2, 25, 'l');
	disp_ch(2, 30, ':');

	disp_ch(2, 40, temp_a);
	disp_ch(2, 45, temp_b);
	disp_ch(2, 50, 0x7F);
	disp_ch(2, 55, 'F');
	
}

void main(void)
{
	init_lcd();	
	refresh_screen();
	init_adc0();



	IE = 0xA0;
	RCAP2H = 0xF8;
	RCAP2L = 0xCD;
	TH2 = 0xF8;
	TL2 = 0xCD;
	T2CON = 0x04;

	blank_screen();
	refresh_screen();

	while(1)
	{
		while(!flag);
		flag = 0;
		potval *= 31;
		potval = potval >> 12;
		potval += 55;

		tempval -= 2475;
		tempval *= 12084;
		tempval = tempval >> 16;

		if(tempval < potval)
		{
			LED = 0x00;
			led8 = 0;
			led9 = 0;
		}
		else
		{
			LED = 0xFF;
			led8 = 1;
			led9 = 1;
		}



		blank_screen();
		display();

		refresh_screen();
	}

}

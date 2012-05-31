#include "REG52.h"
#include <intrins.h>
#include "own_reg.h"
sbit key_BL = P2 ^ 7;

void Timer_Init_0_2ms()
{
	TMOD &= 0xF0;
	TMOD |= 0x01;
	TH0 = 0x4C;
	TL0 = 0x00;
	EA = 1;
	ET0 = 1;
	TR0 = 1;

}

void Timer1_Init_0_2ms()
{
	//	TMOD = 0x02;
	//	TH1 = 0x48;//0.1ms
	//	TL1 = 0x48;
	//	TMOD = 0x21;
	/*
	 TMOD &= 0x0F;
	 TMOD |= 0x21;
	 EA = 1;
	 ET1 = 1;
	 TR1 = 1;   */

}

void SW_Reset()
{
	IAP_CONTR = 0x20;
}

void SW_DL_Reset()
{
	IAP_CONTR = 0x60;
}

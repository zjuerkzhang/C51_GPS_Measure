//#include"STC.H" 
#include <reg52.h>
#include "display.h"
#include "intFUC.h"
#include "gps.h"

#define MCU_POWER_DOWN  PCON |= 0x02 
#define MCU_IDLE        PCON |= 0x01  
#define MCU_CLEAR_POF   PCON &= 0xEF 
unsigned int Power_Up_Count = 0;
sbit Power_key   = P3^2; 
sbit Led_Flash =  P3^5;	
sbit LCD_BL = P3^4; 
void ShutDown();

void INTInit()
{
	EA      = 1;   
	EX0    = 1;   
	
 	IT0      = 1;   
	EX1    = 1;   
	IT1	=1;	
}

void INT0_int() interrupt 0
{

   	unsigned char i;
	for(i = 0;i<150;i++)
 	{
		delay_ms(70);
		if(Power_key)
		{
			i = 0;
			CloseGpsPower();
			Led_Flash = 1;
			INTInit();
			ShutDown();	

		}
	}

	SW_DL_Reset();
}


void ShutDown() 
{
	LCD_BL = 0;//
	MCU_POWER_DOWN;	
}
#if 0

void PowerUp_Count()
{
	
	SW_DL_Reset();
}
#endif
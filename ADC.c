#include <reg52.h>
#include <intrins.h>
#include "display.h"
#include "own_reg.h"


#define  ADC_POWER   0x80  
#define	 ADC_FLAG    0x10
#define	 ADC_START   0x08
#define	 ADC_SPEEDLL 0x00
#define	 ADC_SPEEDL	 0x20
#define	 ADC_SPEEDH	 0x40
#define	 ADC_SPEDHH  0x60
sbit Led_Flash =  P3^5;	
extern unsigned int BatQuan; 
extern unsigned int BatQuan_LST;
extern unsigned char ADCQuaValue;
extern bit BatQuanF;
extern unsigned int  TEST_7;	
void InitADC();

unsigned char GetADCResult(unsigned char ch);

unsigned char GetADCResult(unsigned char ch)
{
	ADC_CONTR=ADC_POWER|ADC_SPEEDLL|ch|ADC_START;
	_nop_();	
	_nop_();
	_nop_();
	_nop_();
	while(!(ADC_CONTR&ADC_FLAG));
//	Led_Flash = ~Led_Flash;
	ADC_CONTR&=~ADC_FLAG;
	return ADC_RES;  
}
void InitADC()
{
	P1ASF=0x01;
	ADC_RES=0;
	ADC_CONTR=ADC_POWER|ADC_SPEEDLL;  

	delay_ms(100);
}

void ADC2BATVALUE()
{

 	if(ADCQuaValue >= 0xCB)
 		BatQuan = 4;
 	else if(ADCQuaValue >= 0xC1)
 	{
 		BatQuan = 3;
 		if(TEST_7 == 0)
 			TEST_7 =1;
 	}
 	else if(ADCQuaValue >= 0xBC)
 	{
 		BatQuan = 2;
 		if(TEST_7 == 0)
 			TEST_7 =1;
 	}
 	else if(ADCQuaValue >= 0xB2)
 	{
 		BatQuan = 1;
 		if(TEST_7 == 0)
 			TEST_7 =1;
 	}
 	else if(ADCQuaValue >= 0xAC)
 	{
 		BatQuan = 0;
 		if(TEST_7 == 0)
 			TEST_7 =1;
 	}
 	else if(ADCQuaValue < 0xAC)
 	{
 		BatQuan = 5;
 		if(TEST_7 == 0)
 			TEST_7 =1;
 	}

 	if(BatQuan_LST != BatQuan)
 	{
 		BatQuanF = 1;
 		BatQuan_LST =  BatQuan;
 	}
 	else
 		BatQuanF = 0;
}
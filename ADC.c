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
unsigned int BatQuan = 0;
unsigned int BatQuan_LST;
unsigned char ADCQuaValue = 0;
void InitADC();

void GetADCResult(unsigned char ch);

void GetADCResult(unsigned char ch)
{
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	while (!(ADC_CONTR & ADC_FLAG))
		;
	ADC_CONTR &= ~ADC_FLAG;
	ADCQuaValue = ADC_RES;
}
void InitADC()
{
	P1ASF = 0x01;
	ADC_RES = 0;
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL;

	delay_ms(100);
}

void ADC2BATVALUE()
{

	if (ADCQuaValue >= 0xCB)
		BatQuan = 4;
	else if (ADCQuaValue >= 0xC1)
	{
		BatQuan = 3;
	}
	else if (ADCQuaValue >= 0xBC)
	{
		BatQuan = 2;
	}
	else if (ADCQuaValue >= 0xB2)
	{
		BatQuan = 1;
	}
	else if (ADCQuaValue >= 0xAC)
	{
		BatQuan = 0;
	}
	else if (ADCQuaValue < 0xAC)
	{
		BatQuan = 5;
	}

	if (BatQuan_LST != BatQuan)
	{
		BatQuan_LST = BatQuan;
	}
}

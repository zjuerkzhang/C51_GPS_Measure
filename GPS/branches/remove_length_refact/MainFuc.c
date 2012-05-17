#include "reg52.h"
//#include "st7565.h"
#include "keyfuc.h"
#include "display.h"
//#include "font.h"
#include "own_reg.h"
#include "IntFUC.h"
#include "string.h"
#include "gps.h"
#include "sleep.h"
#include "adc.h"
#include "stdio.h"
#include "area.h"
#include "own_reg.h"
#include "e2prom.h"

//LED FLASH
# define uint unsigned int
#define TimerTerminal 20
#define GpsGetCount 1000
 uint TimerNumber = 0;
 sbit Led_Flash =  P3^5;	 //
 bit LCD_Fresh = 0 ;
 extern bit TEST4;
//extern unsigned char  KeyPressValue;
extern unsigned int TEST_5;
extern bit FLAG3;
extern unsigned char TEST1;
//unsigned char test[]="hi,hello";
extern unsigned char curr_time[6]; 
extern unsigned char JD[10]; 
extern unsigned char WD[9]; 
extern unsigned char lock; 
extern unsigned char use_sat[3]; 
extern unsigned char total_sat[3];
extern bit GPS_UPDATA;
extern bit GPS_TIME_UPDATE;
extern bit TOTAL_SAT_UPDATE;
extern unsigned char TEST_1[5];
extern unsigned int PowerDownCount;
extern unsigned int BatQuan; 
extern unsigned char FLAG1;
extern unsigned char sn_string[SYSTEM_DATA_SIZE];

unsigned char debugF =0;
sbit Power_key   = P3^2;
sbit LCD_BL = P3^4;
unsigned char ADCQuaValue = 0;
//unsigned char Gra_Buffer[1024];
unsigned int StarNum = 0;
unsigned char TEST_9[20];
unsigned char GetLenthValue[20] ;
extern bit gps_first_point;	
extern double testNum;
extern bit GPS_Point_Updata_WD;
extern bit GPS_Point_Updata_JD;
extern bit GPS_Point_Updata_SatNum;
extern bit danwei_zouchang_sel;	
extern bit danwei_mianji_sel;
extern bit Cacul_GoOn_F;
extern unsigned char celiangPage_idx;
unsigned int key_press_count = 0;
unsigned char clear_rec_code[7] = {0, 2, 2, 4, 4, 5, 5};
unsigned char clear_rec_step;

void PowerUpSeque()
{


   	P3M0 |= 0x10;
	P3M1 &= 0xef;
	LCD_BL = 1;

	P1M0 |= 0x20;
	P1M1 &= 0xdf;
	OpenGpsPower();
	Initial_lcd();
	display_LOGO();
	TEST1 = 0;
	Timer_Init_0_2ms();

	gps_init();
	InitADC();
	ADCQuaValue = GetADCResult(0);
	
}

void initiate_var()
{
	unsigned char i;
	for(i=0;i<20;i++)
	{
		TEST_9[i] = '0';
		GetLenthValue[i] = '0';
	}

	for(i=0;i<10;i++)
	{
		JD[i] = '0';
	}
	for(i=0;i<9;i++)
	{
		WD[i] = '0';
	}
	clear_rec_step = 0;
}

void main()
{
  
	unsigned char test;	
	unsigned char EppromAddrH,EppromAddrL;
	unsigned int TestNumber = 0;
	CRDGEODETIC point, out_point;
	bit timer_fresh = 0;

	PowerUpSeque();
	initiate_var();

	init_history_data();
	get_sn_data();

	FLAG3 = 1;

	while(1) 
	{
		if(GPS_TIME_UPDATE == 1)
		{
			UTC2BeiJingTime();
			GPS_TIME_UPDATE = 0;
			FLAG3 = 1;
		}

		if(GPS_UPDATA == 1)
		{
			if(((use_sat[0]>='0')&&(use_sat[0]<='9'))&&((use_sat[1]>='0')&&(use_sat[1]<='9')))
				StarNum = (use_sat[0]-0x30)*10+(use_sat[1]-0x30);
			else
				StarNum = 0;

			if ((1 == TEST1)&&(1 == FLAG1)&&('1' == lock))
			{
				MakeGeodeticByString(&point, JD, WD);

				if (gps_first_point) //如果是第一个点的话。
				{
					if (GeodeticPutSamplingPoint(&point, &out_point))
					{
						//	Led_Flash = 0;
						GeodeticFirstPoint(&out_point);
						GeodeticResetSamplingPoint();
						gps_first_point = 0;
					}
				}
				else
				{
					GeodeticNextPoint(&point);
				}

				if(0 == danwei_mianji_sel)	//亩
					sprintf(TEST_9, "%08.1f", GeodeticGetArea()/666.666667);
				else
					sprintf(TEST_9, "%08.1f", GeodeticGetArea()/10000);

				if(0 == danwei_zouchang_sel) //米
				{
					sprintf(GetLenthValue, "%08.0f", GeodeticGetDistance());
				}
				else
				{
					sprintf(GetLenthValue, "%08.1f", GeodeticGetDistance()/1000);
				}
			}

			GPS_UPDATA = 0;
			FLAG3 = 1;
		}

		if(TOTAL_SAT_UPDATE == 1)
		{

		}

		if(PowerDownCount>40)
		{
			display_PowerD_LOGO();
		}

		if(keyscan())
		{
			KeyOperate();
			key_press_count = 0;
		}
		else
		{
			if( 1==TEST1 && CELIANG_SN_PAGE==celiangPage_idx)
			{
				celiangPage_idx = CELIANG_WORKING_PAGE;
				FLAG3 = 1;
			}
		}
		if((FLAG3 == 1))
		{
			if(TimerNumber >= TimerTerminal)
			{
				TimerNumber = 0;
				timer_fresh = 1;
			}
			else
				timer_fresh = 0;

			FLAG3 = 0;

			switch(TEST1)
			{
			case 0:
				display_Idle();
				break;

			case 1:
				display_CeLiang_Page(timer_fresh);
				break;

			case 2:
				display_danjia_Page();
				break;

			case 3:
				display_danwei_Page();
				break;

			case 4:
				display_jilu_page();
				break;

			case 5:
				dispay_sn_edit_page();
				break;

			default:
				break;
			}
		}

		wait_key_ok_release();

		delay_ms(200);
	} 	 
	
}

 void timer0() interrupt 1
 {
	 ET0 = 0;
	 TR0 = 0;

	 key_press_count++;
	 if(key_press_count >= 400)	// 20s
	 {
		 key_press_count = 0;
		 LCD_BL = 0;
	 }

	 if(!Power_key)
	 {
		 PowerDownCount++;
	 }
	 else
	 {
		 if(PowerDownCount >= 1)
		 {
			 PowerDownCount = 0;
			 LCD_BL = ~LCD_BL;
		 }
		 else
		 {
			 PowerDownCount = 0;
		 }
	 }

	 TimerNumber++;
	 TH0 = 0x4C;
	 TL0 = 0x00;
	 EA = 1;
	 ET0 = 1;
	 TR0 = 1;
 }

 void timer1() interrupt 3
 {


 }

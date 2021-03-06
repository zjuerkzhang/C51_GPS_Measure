#include "reg52.h"
//#include "st7565.h"
#include "display.h"
//#include "font.h"
#include "own_reg.h"
#include "IntFUC.h"
#include "keyfuc.h"
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
#define TimerTerminal 10
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
extern unsigned char TEST_1[5];
extern unsigned int PowerDownCount;
extern unsigned int BatQuan; 
extern unsigned char FLAG1;
unsigned char debugF =0;
sbit Power_key   = P3^2;
sbit LCD_BL = P3^4;
unsigned char ADCQuaValue = 0;
//unsigned char Gra_Buffer[1024];
unsigned int StarNum = 0;
unsigned char TEST_9[20];
unsigned char GetLenthValue[20] ;
extern bit gps_first_point;	
unsigned int GpsAreaGet = 0;
extern double testNum;
extern bit GPS_Point_Updata_WD;
extern bit GPS_Point_Updata_JD;
extern bit GPS_Point_Updata_SatNum;
extern bit danwei_zouchang_sel;	
extern bit danwei_mianji_sel;
extern bit Cacul_GoOn_F;
unsigned int key_press_count = 0;

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

void main()
{
  
 /*  	P3M0 |= 0x10;
	P3M1 &= 0xef;
//	LCD_BL_Ctl_fuc();

//	OpenGpsPower();
	Initial_lcd();
	INTInit();
	LcmClear();	
//	display_chinesestring();
	Timer_Init_0_2ms();
//	Timer1_Init_0_2ms();
   
*/
	unsigned char test;	
	unsigned char EppromAddrH,EppromAddrL;
	unsigned int TestNumber = 0;

	PowerUpSeque();
//	display_bat();

	init_history_data();
#if 0
 	 TestNumber= 0;
	 for(EppromAddrH = 0x20;EppromAddrH <=0x23;EppromAddrH++)
	 {
	 	for(EppromAddrL = 0x00;EppromAddrL<0xff;EppromAddrL++)
		{
			EEPROM_write(TestNumber,EppromAddrH,EppromAddrL);
		//	
			delay_ms(100);
		//	Led_Flash = 0;	
		}
		TestNumber++;
	 }
//	 Led_Flash = 0;
	 for(EppromAddrH = 0x20;EppromAddrH <=0x23;EppromAddrH++)
	 {
	 	for(EppromAddrL = 0x00;EppromAddrL<0xff;EppromAddrL++)
		{


			delay_ms(100);
				
		}

	 }

#endif


#if 0
	DataSend_char(EEPROM_Read(0x00));
	DataSend("\r\n") ;
	eeprom_eares(0x0000);
	EEPROM_write(0x0000,'8');

//	uchar EEPROM_Read(uchar EEPROM_Addr)
	test = EEPROM_Read(0x0000);
	DataSend_char(test);

	DataSend("\r\n") ;
 #endif
	while(1) 
	{



	if(GPS_UPDATA == 1)
	{
	delay_ms(200);
	//ADCQuaValue = GetADCResult(0);

	//ADC2BATVALUE();


	 UTC2BeiJingTime();
	 GPS_UPDATA = 0;
	 FLAG3 = 1;
	}  
	if(PowerDownCount>40)
		{
		//	LCD_BL = 0;
			display_PowerD_LOGO();	
		}

	} 	 

 /*//TEST
{
	gps_init();
	DataSend("TEST");
	
}
*/		
}

 void timer0() interrupt 1
 {


			CRDGEODETIC point, out_point;
		//	char buf[20];

 			
			ET0 = 0;
			TR0 = 0; 


		if(((use_sat[0]>='0')&&(use_sat[0]<='9'))&&((use_sat[1]>='0')&&(use_sat[1]<='9')))
		StarNum = (use_sat[0]-0x30)*10+(use_sat[1]-0x30);
		else
		StarNum = 0;


	//	if ((1 == TEST1)&&(((use_sat[0]-0x30)*10+ (use_sat[1]-0x30)>3)))
		if ((1 == TEST1)&&(('1' == lock))&&(Cacul_GoOn_F == 0)&&(1 == FLAG1))
		{
		  GpsAreaGet++;
		   //Led_Flash = 0;
		  //if(GpsAreaGet>=GpsGetCount)
		  //if((GpsAreaGet>=GpsGetCount)&&(GPS_Point_Updata_WD == 1)&&((GPS_Point_Updata_JD == 1))&&(GPS_Point_Updata_SatNum == 1)&&(((use_sat[0]-0x30)*10+ (use_sat[1]-0x30)>3)))
		  //if((GpsAreaGet>=GpsGetCount)&&(GPS_Point_Updata_JD == 1))
		  if(GPS_Point_Updata_JD == 1)
		  {
		  		 
		  	//	EA = 0; 
		  	//	 Led_Flash = ~Led_Flash;
				//sprintf(buf, "%d", GPS_Point_Updata);
				//DataSend("\r\n") ;
				//if (GPS_Point_Updata==1)DataSend("1");
				//else if(GPS_Point_Updata==0)DataSend("0");
				//else DataSend("xx");
			//	DataSend_char(GPS_Point_Updata);
			//	DataSend("\r\n") ;

				GPS_Point_Updata_WD = 0;
				GPS_Point_Updata_JD = 0;
				GPS_Point_Updata_SatNum = 0;

//				DataSend("\r\n--JD:");
//				DataSend(JD);
//				DataSend("\r\n--WD:");
//				DataSend(WD);

				MakeGeodeticByString(&point, JD, WD);
			#if 1	
				if (gps_first_point) //如果是第一个点的话。
				{
					
					if (GeodeticPutSamplingPoint(&point, &out_point)) {
					//	Led_Flash = 0;
						GeodeticFirstPoint(&out_point);
						GeodeticResetSamplingPoint();
						gps_first_point = 0;
					}
					
				}
				else
			#endif
				{
					GeodeticNextPoint(&point);
				}
				if(0 == danwei_mianji_sel)	//亩
				  sprintf(TEST_9, "%08.1f", GeodeticGetArea()/666.666667);
				else
				   sprintf(TEST_9, "%08.1f", GeodeticGetArea()/10000);
					
//					 DataSend("\r\n") ;
//					 DataSend(TEST_9);
//					 DataSend("\r\n") ;
				if(0 == danwei_zouchang_sel) //米
				{
					sprintf(GetLenthValue, "%08.0f", GeodeticGetDistance());
				}
				else
				{
					sprintf(GetLenthValue, "%08.1f", GeodeticGetDistance()/1000);
				}
//					 DataSend("\r\n") ;
//					 DataSend(GetLenthValue);
//					 DataSend("\r\n") ;

		  	   GpsAreaGet=0;
  ///////////////////////////////////////////
  /*
			   Doubletest();
			   sprintf(buf, "%f", testNum);
				DataSend("\r\n") ;
				DataSend(buf);
				DataSend("\r\n") ;	*/

		  }
		 }	  
		 if(keyscan()) 
		{
			KeyOperate();
			key_press_count = 0;
		}
		 else
		 {
			 key_press_count++;
			 if(key_press_count >= 400)	// 20s
			 {
				 key_press_count = 0;
				 LCD_BL = 0;
			 }
		 }

		if(!Power_key) 
		{
			PowerDownCount++;
		}
		else if(PowerDownCount >= 1)
		{
			PowerDownCount = 0;
		   LCD_BL = ~LCD_BL;
		}
		else
		{
			PowerDownCount = 0;
		}

	   //
	   if(TimerNumber >= TimerTerminal)
	   {

	   	  TimerNumber = 0;
		switch(TEST1)
		{ 
				
			case 0:
			display_Idle();			
			break;
			case 1:
			display_CeLiang_Page();
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
			default:
			break;
		}

		}
	  	 TimerNumber ++;
	  	TH0 = 0x4C;
	  	TL0 = 0x00;
	   	EA = 1;
		ET0 = 1;
		TR0 = 1;
 }

 void timer1() interrupt 3
 {


 }

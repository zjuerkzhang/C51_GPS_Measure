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

sbit CHG_F1_STDBY = P1 ^ 7;
sbit Power_key = P3 ^ 2;
sbit LCD_BL = P3 ^ 4;

bit g_lcd_refresh = 0;
unsigned char g_page_id = 0;
unsigned int PowerDownCount = 0;
unsigned int StarNum = 0;
unsigned char g_area_value[20];
unsigned char g_length_value[20];
unsigned int key_press_count = 0;
unsigned char battery_timer_count = 40;
bit time_valid_flag = 0;
unsigned char height[10];
unsigned char width[10];

extern unsigned char JD[10];
extern unsigned char WD[9];
extern unsigned char use_sat[3];
extern bit GPS_UPDATA;
extern bit GPS_TIME_UPDATE;
extern bit TOTAL_SAT_UPDATE;
extern bit gps_first_point;
extern bit danwei_zouchang_sel;
extern bit danwei_mianji_sel;
extern unsigned char celiangPage_idx;
extern bit signal;
extern unsigned int BatQuan;
extern unsigned char celiang_mode;
extern unsigned char danwei_sel;
extern unsigned char ruler_mode;

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
	g_page_id = 0;
	Timer_Init_0_2ms();

	gps_init();
	InitADC();
	GetADCResult(0);

}

void main()
{

	unsigned char EppromAddrH, EppromAddrL;
	unsigned int TestNumber = 0;
	CRDGEODETIC point;
	bit timer_fresh = 0;

	PowerUpSeque();
	initiate_var(1);
	initiate_ruler_var();

	init_history_data();
	get_sn_data();

	g_lcd_refresh = 1;

	while (1)
	{
		if (GPS_TIME_UPDATE == 1 && time_valid_flag == 1)
		{
			UTC2BeiJingTime();
			GPS_TIME_UPDATE = 0;
			timer_fresh = 1;
			g_lcd_refresh = 1;
		}

		if (GPS_UPDATA == 1)
		{
			if (((use_sat[0] >= '0') && (use_sat[0] <= '9')) &&
				((use_sat[1] >= '0') && (use_sat[1] <= '9')))
				StarNum = (use_sat[0] - 0x30) * 10 + (use_sat[1] - 0x30);
			else
				StarNum = 0;

			if (signal)
				time_valid_flag = 1;

			if ((1 == g_page_id) && (1 == celiang_mode) && (signal))
			{
				MakeGeodeticByString(&point, JD, WD);

				if (gps_first_point) //如果是第一个点的话。
				{
					if (PreparePointPlusArray(&point))
					{
						GeodeticFirstPoint();
						gps_first_point = 0;
					}
				}
				else
				{
					GeodeticNextPointPlus(&point);
				}

				if (1 == danwei_sel) //公顷
					sprintf(g_area_value, "%08.2f", GeodeticGetArea()/10000);
				else //默认面积单位为亩
					sprintf(g_area_value, "%08.2f", GeodeticGetArea()/666.666667);

				if (3 == danwei_sel) //千米
					sprintf(g_length_value, "%08.1f", GeodeticGetDistance()/1000);
				else // 默认长度单位为米
					sprintf(g_length_value, "%08.1f", GeodeticGetDistance());

			}

			if ((3==g_page_id) && (1==ruler_mode || 3==ruler_mode) && (signal) )
			{
				MakeGeodeticByString(&point, JD, WD);
				if (gps_first_point) //如果是第一个点的话。
				{
					if (PrepareStartPointArray(&point))
					{
						GetLineStartPoint();
						gps_first_point = 0;
					}
				}
				else
				{
					GetNextLinePoint(&point);

					if (1==ruler_mode)
						sprintf(height, "%06.1f", GetLineLength());
					else if(3==ruler_mode)
						sprintf(width, "%06.1f", GetLineLength());
				}
			}

			GPS_UPDATA = 0;
			g_lcd_refresh = 1;
		}

		if (TOTAL_SAT_UPDATE == 1)
		{

		}

		if (battery_timer_count >= 30) // 1.5s to get new battery value
		{

			if ((CHG_F1_STDBY == 0))
			{
				BatQuan++;
				if (BatQuan > 4)
				{
					BatQuan = 1;
				}
			}
			else
			{
				GetADCResult(0);
				ADC2BATVALUE();
			}
			battery_timer_count = 0;
			g_lcd_refresh = 1;
		}

		if (PowerDownCount > 40)
		{
			display_PowerD_LOGO();
		}

		if (keyscan())
		{
			KeyOperate();
			key_press_count = 0;
		}
		else
		{
			if (1 == g_page_id && (CELIANG_SN_PAGE == celiangPage_idx
					|| CELIANG_DETAIL_PAGE == celiangPage_idx))
			{
				celiangPage_idx = CELIANG_WORKING_PAGE;
				g_lcd_refresh = 1;
			}
		}
		if (g_lcd_refresh)
		{
			g_lcd_refresh = 0;

			switch (g_page_id)
			{
			case 0:
				display_Idle();
				break;

			case 1:
				display_CeLiang_Page(timer_fresh);
				timer_fresh = 0;
				break;

			case 2:
				display_danjia_Page();
				break;

			case 3:
				display_ruler_page(timer_fresh);
				timer_fresh = 0;
				//display_danwei_Page();
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

void
timer0() interrupt 1
{
	ET0 = 0;
	TR0 = 0;

	key_press_count++;
	if(key_press_count >= 400) // 20s
	{
		key_press_count = 0;
		LCD_BL = 0;
	}

	if(!Power_key)
	{
		PowerDownCount++;
		key_press_count = 0;
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

	battery_timer_count++;

	TH0 = 0x4C;
	TL0 = 0x00;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

void
timer1() interrupt 3
{

}

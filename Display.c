#include "REG52.h"
#include <intrins.h>
#include"font.h"
#include "gps.h"
#include "sleep.h"
#include "adc.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "e2prom.h"
#include "keyfuc.h"

#define  uchar  unsigned char
#define  uint   unsigned int

#define VC_ON 0x2C 
#define VR_ON 0x2A   
#define VF_ON 0x29 

#define NOP _nop_();_nop_();_nop_();_nop_()  
#define data_bus   P0 

sbit CS = P1 ^ 2;
sbit RES = P1 ^ 4;
sbit A0 = P1 ^ 3;
sbit LCD_WR = P3 ^ 6;
sbit LCD_RD = P3 ^ 7;

sbit DB0 = P0 ^ 0;
sbit DB1 = P0 ^ 1;
sbit DB2 = P0 ^ 2;
sbit DB3 = P0 ^ 3;
sbit DB4 = P0 ^ 4;
sbit DB5 = P0 ^ 5;
sbit DB6 = P0 ^ 6;
sbit DB7 = P0 ^ 7;

sbit CS_LCD = P1 ^ 2;
sbit AO = P1 ^ 3;
sbit RST = P1 ^ 4;

sbit Led_Flash = P3 ^ 5;

sbit d1 = P1 ^ 0;
sbit d2 = P1 ^ 1;

unsigned char curr_history_idx;
unsigned char menu_focus_idx = 0;
double total_cost = 0;
unsigned char total_cost_str[20];
unsigned char price_per_area[] =
{ 0, 0, 8, 0, 0 };
unsigned char price_per_len[] =
{ 0, 0, 0, 0, 0 };
unsigned char celiang_run_light = 0;
unsigned char danwei_sel = 0;
unsigned char *g_measure_for_show;

extern unsigned int BatQuan;
extern unsigned char celiang_mode;
extern bit signal;
extern unsigned char danjia_sel_focus;
extern bit danwei_zouchang_sel;
extern bit danwei_mianji_sel;
extern bit FLAG4;
extern unsigned char g_beijing_time[7];
extern unsigned char use_sat_LST[3];
extern unsigned char total_sat_LST[3];
extern unsigned char JD[10];
extern unsigned char WD[9];
extern unsigned char celiangPage_idx;
extern unsigned int StarNum;
extern unsigned char g_area_value[];
extern unsigned char g_length_value[];
extern bit gps_first_point;
extern bit searching_sat;
extern unsigned char sn_focus_idx;
extern unsigned char system_data[SYSTEM_DATA_SIZE];
extern unsigned char price_for_edit[];
extern unsigned char danwei_sel_for_edit;
extern unsigned char len_moving[];
extern unsigned char spa_len;

void delay_ms(unsigned int ms)
{
	unsigned int n;
	while (ms--)
	{
		n = 86;
		while (n--)
			;
	}
}

void LCD_Reset()
{
	IE = 0x00;
	RES = 0;
	delay_ms(50);
	RES = 1;
	delay_ms(50);
}

void Write_Data(uchar dat)
{

	CS = 0;
	A0 = 1;
	data_bus = dat;
	LCD_WR = 0;
	LCD_RD = 1;
	CS = 1;
}

#if 0 
void Write_Data_rail(uchar dat)
{
	CS = 1;
	A0 = 1;

	LCD_WR = 0;
	LCD_RD = 1;
	data_bus = dat;
	CS = 1;
}
#endif

#if 0  
uchar Read_Data(void)
{
	CS = 0;
	return data_bus;
	A0 = 1;
	LCD_WR = 1;
	LCD_RD = 0;
	CS = 1;
}
#endif 

void Write_Instruction(uchar cmd)
{
	CS = 0;
	data_bus = cmd;
	A0 = 0;
	LCD_WR = 0;
	LCD_RD = 1;
	CS = 1;
}
void Set_Page_Address(unsigned char page)
{
	page = 0xb0 | page;
	Write_Instruction(page);
	return;
}
void Set_Column_Address(unsigned char Column)
{
	uchar j;
	j = Column;
	Write_Instruction((0x10 | (j >> 4)));
	Write_Instruction((0x0f & Column));
	return;
}

void LcmClear(void)
{
	uchar i, j;
	Write_Instruction(0x40);
	for (i = 0; i < 8; i++)
	{
		Write_Instruction(0xB0 | i);
		Write_Instruction(0x10);
		Write_Instruction(0x00);
		for (j = 0; j < 128; j++)
		{
			Write_Data(0x00);
		}
	}
}
#if 0
void DisplayCGRAM(unsigned char x,unsigned char y)
{
	switch(y)
	{
		case 1: Write_Instruction(0x80+x);break;
		case 2: Write_Instruction(0x90+x);break;
		case 3: Write_Instruction(0x88+x);break;
		case 4: Write_Instruction(0x98+x);break;
		default:break;
	}
	Write_Data(00);
	Write_Data(00);
}
#endif

void Revers_Data(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num;

	Set_Page_Address(page);
	Set_Column_Address(column);
	num = 16;
	for (k = 0; k < 16; k++)
	{
		Write_Data(~ft[num + k]);

	}

	Set_Page_Address(page + 1);
	Set_Column_Address(column);
	num = 0;
	for (k = 0; k < 16; k++)
	{

		Write_Data(~ft[num + k]);
	}

}

void Initial_lcd(void)
{
	CS = 0;
	LCD_Reset();
	Write_Instruction(0xE2);
	Write_Instruction(0xA2);
	Write_Instruction(0xA0);
	Write_Instruction(0xC0);
	Write_Instruction(0xa6);
	Write_Instruction(0x26);
	Write_Instruction(0xa4);
	Write_Instruction(0xE5);
	Write_Instruction(0xD2);
	Write_Instruction(0x01);
	Write_Instruction(0x81);

	Write_Instruction(0x00);
	Write_Instruction(0x2f);
	Write_Instruction(0xAF);
	delay_ms(1);
}

void Display_Chinese(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	num = 16;
	for (k = 0; k < 16; k++)
	{
		Write_Data(ft[num + k]);
	}

	Set_Page_Address(page + 1);
	Set_Column_Address(column);
	num = 0;
	for (k = 0; k < 16; k++)
	{
		Write_Data(ft[num + k]);
	}

	return;
}
void Display(unsigned char *p)
{
	unsigned char page, column;
	for (page = 0xB7; page >= 0xB0; page--)
	{
		Write_Instruction(page);
		Write_Instruction(0x10);
		Write_Instruction(0x00);
		for (column = 0; column < 128; column++)
		{
			Write_Data(*p++);
		}
		delay_ms(7000);
	}
	return;
}
void Display_PD(unsigned char *p)
{
	unsigned char page, column;
	for (page = 0xB7; page >= 0xB0; page--)
	{
		Write_Instruction(page);
		Write_Instruction(0x10);
		Write_Instruction(0x00);
		for (column = 0; column < 128; column++)
		{
			Write_Data(*p++);
		}

	}
	return;
}
#if 0

void Display_bank(unsigned char *p)
{
	unsigned char page,column;
	for(page=0xB0;page>=0xB1;page++)
	{
		Write_Instruction(page);
		Write_Instruction(0x10);
		Write_Instruction(0x00);
		for(column=0;column<16;column++)
		{
			Write_Data(*p++);
		}
	}
	return;
}
#endif

#if 0

void Display_Chinese_2(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	num=87;
	for(k=0;k<29;k++)
	{
		Write_Data(ft[num+k]);
	}
	Set_Page_Address(page+1);
	Set_Column_Address(column);
	num=58;
	for(k=0;k<29;k++)
	{
		Write_Data(ft[num+k]);
	}
	Set_Page_Address(page+2);
	Set_Column_Address(column);
	num=29;
	for(k=0;k<29;k++)
	{
		Write_Data(ft[num+k]);
	}
	Set_Page_Address(page+3);
	Set_Column_Address(column);
	num=0;
	for(k=0;k<29;k++)
	{
		Write_Data(ft[num+k]);
	}
	return;
}
#endif

void zf_disp8x16(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	num = 8;
	for (k = 0; k < 8; k++)
	{
		Write_Data(ft[num + k]);
	}

	Set_Page_Address(page + 1);
	Set_Column_Address(column);
	num = 0;
	for (k = 0; k < 8; k++)
	{
		Write_Data(ft[num + k]);
	}

}

void zf_disp7x16(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	num = 7;
	for (k = 0; k < 7; k++)
	{
		Write_Data(ft[num + k]);
	}

	Set_Page_Address(page + 1);
	Set_Column_Address(column);
	num = 0;
	for (k = 0; k < 7; k++)
	{
		Write_Data(ft[num + k]);
	}

}

void reverse_zf_disp8x16(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	num = 8;
	for (k = 0; k < 8; k++)
	{
		Write_Data(~ft[num + k]);
	}

	Set_Page_Address(page + 1);
	Set_Column_Address(column);
	num = 0;
	for (k = 0; k < 8; k++)
	{
		Write_Data(~ft[num + k]);
	}

}

void zf_clear_page_to_end(uchar page, uchar column, uchar fill_data)
{
	unsigned char k;
	Set_Page_Address(page);
	Set_Column_Address(column);
	for (k = column; k < 128; k++)
	{
		Write_Data(fill_data);
	}
}

void zf_disp8x8(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;
	num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	for (k = 0; k < 8; k++)
	{
		Write_Data(ft[num + k]);
	}
}

void zf_disp16x8(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;
	;
	num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	for (k = 0; k < 16; k++)
	{
		Write_Data(ft[num + k]);
	}
}

#if 0

void reverse_zf_disp16x8(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;;
	num=0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	for(k=0;k<16;k++)
	{
		Write_Data(~ft[num+k]);
	}
}
#endif

void zf_disp4x8(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;

	num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	for (k = 0; k < 4; k++)
	{
		Write_Data(ft[num + k]);
	}
}

void Display_xinhao(unsigned char ft[], uchar page, uchar column)
{
	unsigned char k, num = 0;
	Set_Page_Address(page);
	Set_Column_Address(column);
	num = 0;
	for (k = 0; k < 16; k++)
	{
		Write_Data(ft[num + k]);
	}
	Set_Page_Address(page + 1);
	Set_Column_Address(column);
	num = 16;
	for (k = 0; k < 16; k++)
	{
		Write_Data(ft[num + k]);
	}
	return;
}

void display_chinesestring()
{

	Display(logo);
	delay_ms(3000);

	LcmClear();

	Display_Chinese(num3, 1, 0);
	Display_Chinese(dan, 1, 16);
	Display_Chinese(wei, 1, 32);
	Display_Chinese(kong, 1, 48);

	Display_Chinese(num4, 1, 64);
	Display_Chinese(ji, 1, 80);
	Display_Chinese(lu, 1, 96);

	Display_Chinese(kong, 1, 112);
	Display_Chinese(kong, 1, 128);

	Display_Chinese(num1, 4, 0);
	Display_Chinese(ce, 4, 16);
	Display_Chinese(liang, 4, 32);

	Display_Chinese(kong, 4, 48);

	Display_Chinese(num2, 4, 64);
	Display_Chinese(dan, 4, 80);
	Display_Chinese(jia, 4, 96);

	Display_Chinese(kong, 4, 112);
	Display_Chinese(kong, 4, 128);

	zf_disp4x8(num[1], 7, 4); //1
	zf_disp4x8(num[2], 7, 8); //1
	zf_disp4x8(num[10], 7, 12); //：
	zf_disp4x8(num[3], 7, 16); //3
	zf_disp4x8(num[0], 7, 20); //0
	Display_Chinese(kong, 7, 24);
	Display_Chinese(kong, 7, 39);
	Display_Chinese(kong, 7, 55);
	Display_Chinese(kong, 7, 71);
	Display_Chinese(kong, 7, 87);
	Display_Chinese(kong, 7, 103);
	zf_disp16x8(xinhao, 7, 110);

	delay_ms(10000);

	Revers_Data(num3, 1, 0);
	Revers_Data(dan, 1, 16);
	Revers_Data(wei, 1, 32);
}
void display_LOGO()
{

	LcmClear();
	Display(logo);
	delay_ms(6000);

}
void display_PowerD_LOGO()
{

	EA = 0;

	Display_PD(PowerD_logo);
	delay_ms(10000);
	CloseGpsPower();

	Led_Flash = 1;

	LcmClear();

	LCD_Reset();
	INTInit();
	ShutDown();

}

void display_danwei( unsigned char l_danwei_sel,
		             unsigned char row,
		             unsigned char col,
		             unsigned char reverse )
{
	switch (l_danwei_sel)
	{
	case 0:
	{
		if (reverse)
			Revers_Data(mu, row, col);
		else
			Display_Chinese(mu, row, col);
		Display_Chinese(kong, row, col+16);
		break;
	}
	case 1:
	{
		if (reverse)
		{
			Revers_Data(gong, row, col);
			Revers_Data(qing, row, col+16);
		}
		else
		{
			Display_Chinese(gong, row, col);
			Display_Chinese(qing, row, col+16);
		}
		break;
	}
	case 2:
	{
		if (reverse)
			Revers_Data(mi, row, col);
		else
			Display_Chinese(mi, row, col);
		Display_Chinese(kong, row, col+16);
		break;
	}
	case 3:
	{
		if (reverse)
		{
			Revers_Data(qian, row, col);
			Revers_Data(mi, row, col+16);
		}
		else
		{
			Display_Chinese(qian, row, col);
			Display_Chinese(mi, row, col+16);
		}
		break;
	}
	default:
		break;
	}
	col += 32;
	for( ; col<128; col+=8 )
	{
		zf_disp8x16(kong, row, col);
	}
}

void display_num_str_8x16( unsigned char p_num_str[],
		                   unsigned char p_length,
		                   unsigned char p_row,
		                   unsigned char p_col )
{
	unsigned char idx;

	for (idx = 0; idx < p_length; idx++)
	{
		if (p_num_str[idx] != 0x2e)
			zf_disp8x16(Num_8_16[p_num_str[idx] - 0x30], p_row, p_col + idx * 8);
		else
			zf_disp8x16(Num_8_16[10], p_row, p_col + idx * 8);
	}
}

void display_non_zero_num_list_8x16( unsigned char p_num_list[],
		                             unsigned char p_length,
		                             unsigned char p_row,
		                             unsigned char p_col,
		                             unsigned char *p_offset)
{
	unsigned char idx;

	*p_offset = 0;
	for (idx = 0; idx < p_length; idx++)
	{
		if (*p_offset > 0)
		{
			zf_disp8x16(Num_8_16[p_num_list[idx]], p_row, p_col + *p_offset);
			*p_offset += 8;
		}
		else
		{
			if (p_num_list[idx] != 0)
			{
				zf_disp8x16(Num_8_16[p_num_list[idx]], p_row, p_col + *p_offset);
				*p_offset += 8;
			}
			else
			{
				if (idx == p_length-1)
				{
					zf_disp8x16(Num_8_16[p_num_list[idx]], p_row, p_col + *p_offset);
					*p_offset += 8;
				}
			}
		}
	}
}


void Update_Idle_page4_5_1_2(unsigned char Sel_flag)
{
	Display_Chinese(num1, 4, 0);
	Display_Chinese(ce, 4, 16);
	Display_Chinese(liang, 4, 32);
	Display_Chinese(kong, 4, 48);
	Display_Chinese(num2, 4, 64);
	Display_Chinese(dan, 4, 80);
	Display_Chinese(jia, 4, 96);
	Display_Chinese(kong, 4, 112);
	Display_Chinese(num3, 1, 0);
	Display_Chinese(dan, 1, 16);
	Display_Chinese(wei, 1, 32);
	Display_Chinese(kong, 1, 48);
	Display_Chinese(num4, 1, 64);
	Display_Chinese(ji, 1, 80);
	Display_Chinese(lu, 1, 96);
	Display_Chinese(kong, 1, 112);

	if ((Sel_flag == 0) || (Sel_flag == 1))
	{
		if (Sel_flag == 0)
		{
			Revers_Data(num1, 4, 0);
			Revers_Data(ce, 4, 16);
			Revers_Data(liang, 4, 32);
			Display_Chinese(kong, 4, 48);
			Display_Chinese(num2, 4, 64);
			Display_Chinese(dan, 4, 80);
			Display_Chinese(jia, 4, 96);
			Display_Chinese(kong, 4, 112);
		}
		else
		{
			Display_Chinese(num1, 4, 0);
			Display_Chinese(ce, 4, 16);
			Display_Chinese(liang, 4, 32);
			Display_Chinese(kong, 4, 48);
			Revers_Data(num2, 4, 64);
			Revers_Data(dan, 4, 80);
			Revers_Data(jia, 4, 96);
			Display_Chinese(kong, 4, 112);
		}
	}
	else if ((Sel_flag == 2) || (Sel_flag == 3))
	{
		if (Sel_flag == 2)
		{
			Revers_Data(num3, 1, 0);
			Revers_Data(dan, 1, 16);
			Revers_Data(wei, 1, 32);
			Display_Chinese(kong, 1, 48);
			Display_Chinese(num4, 1, 64);
			Display_Chinese(ji, 1, 80);
			Display_Chinese(lu, 1, 96);
			Display_Chinese(kong, 1, 112);
		}
		else
		{
			Display_Chinese(num3, 1, 0);
			Display_Chinese(dan, 1, 16);
			Display_Chinese(wei, 1, 32);
			Display_Chinese(kong, 1, 48);
			Revers_Data(num4, 1, 64);
			Revers_Data(ji, 1, 80);
			Revers_Data(lu, 1, 96);
			Display_Chinese(kong, 1, 112);
		}
	}

}
void Update_danjia_page4_5_1_2(unsigned char Sel_flag)
{
	if (Sel_flag == 0)
	{
		Revers_Data(mian, 4, 0);
		Revers_Data(ji2, 4, 16);
	}
	else
	{
		Display_Chinese(mian, 4, 0);
		Display_Chinese(ji2, 4, 16);
	}
	Display_Chinese(maohao, 4, 32);

	if (Sel_flag == 2)
		reverse_zf_disp8x16(Num_8_16[price_for_edit[1]], 4, 48);
	else
		zf_disp8x16(Num_8_16[price_for_edit[1]], 4, 48);

	if (Sel_flag == 3)
		reverse_zf_disp8x16(Num_8_16[price_for_edit[2]], 4, 56);
	else
		zf_disp8x16(Num_8_16[price_for_edit[2]], 4, 56);

	if (Sel_flag == 4)
		reverse_zf_disp8x16(Num_8_16[price_for_edit[3]], 4, 64);
	else
		zf_disp8x16(Num_8_16[price_for_edit[3]], 4, 64);

	Display_Chinese(yuan, 4, 72);
	zf_disp8x16(xie_gang, 4, 88);

	if (Sel_flag == 20)
		display_danwei(danwei_sel_for_edit, 4, 96, 1);
	else
		display_danwei(danwei_sel_for_edit, 4, 96, 0);
}

void Update_danwei_page4_5_1_2(unsigned char l_danwei_sel)
{
	if(l_danwei_sel<2)
	{
		Revers_Data(mian, 4, 0); //面
		Revers_Data(ji2, 4, 16); //积
	}
	else
	{
		Display_Chinese(mian, 4, 0); //面
		Display_Chinese(ji2, 4, 16); //积
	}
	Display_Chinese(maohao, 4, 32); //:

	if (l_danwei_sel == 0)
		Revers_Data(mu, 4, 48);//亩
	else
		Display_Chinese(mu, 4, 48);
	Display_Chinese(kong, 4, 64);

	if (l_danwei_sel == 1)
	{
		Revers_Data(gong, 4, 80); //公
		Revers_Data(qing, 4, 96); //倾
	}
	else
	{
		Display_Chinese(gong, 4, 80);//公
		Display_Chinese(qing, 4, 96);//倾
	}

	Display_Chinese(kong, 4, 112);

	if(l_danwei_sel >1)
	{
		Revers_Data(zou,1,0); //走
		Revers_Data(chang,1,16); //长
	}
	else
	{
		Display_Chinese(zou,1,0); //走
		Display_Chinese(chang,1,16); //长
	}
	Display_Chinese(maohao, 1, 32); //:

	if (l_danwei_sel == 2)
		Revers_Data(mi, 1, 48);//亩
	else
		Display_Chinese(mi, 1, 48);
	Display_Chinese(kong, 1, 64);

	if (l_danwei_sel == 3)
	{
		Revers_Data(qian, 1, 80); //公
		Revers_Data(mi, 1, 96); //倾
	}
	else
	{
		Display_Chinese(qian, 1, 80);//公
		Display_Chinese(mi, 1, 96);//倾
	}

	Display_Chinese(kong, 1, 112);
}


Update_Page_Header()
{
	unsigned char StarNum1, StarNum0;

	StarNum0 = (unsigned char) StarNum % 10;
	StarNum1 = (unsigned char) StarNum / 10;

	zf_disp4x8(num[g_beijing_time[0] - 0x30], 7, 0); //小时1
	zf_disp4x8(num[g_beijing_time[1] - 0x30], 7, 4); //小时2
	zf_disp4x8(num[10], 7, 8); //：
	zf_disp4x8(num[g_beijing_time[2] - 0x30], 7, 12); //分钟1
	zf_disp4x8(num[g_beijing_time[3] - 0x30], 7, 16); //分钟2
	zf_disp4x8(num[10], 7, 20); //：
	zf_disp4x8(num[g_beijing_time[4] - 0x30], 7, 24); //秒钟1
	zf_disp4x8(num[g_beijing_time[5] - 0x30], 7, 28); //秒钟2

	zf_disp8x8(kong8_8, 7, 32);
	zf_disp4x8(kong8_8, 7, 40);

	zf_disp16x8(font5[BatQuan], 7, 44); //电池

	zf_disp8x8(kong8_8, 7, 60);
	zf_disp16x8(kong8_16, 7, 68);

	if (signal)
	{
		zf_disp4x8(num[StarNum1], 7, 84);
		zf_disp4x8(num[StarNum0], 7, 88); //星数量

		zf_disp16x8(kong8_16, 7, 92);
		zf_disp4x8(kong8_4, 7, 108); //kong

		zf_disp16x8(xinhao, 7, 112); //信号
	}
	else
	{
		zf_disp4x8(None8_4, 7, 84); //-
		zf_disp4x8(None8_4, 7, 88); //-

		zf_disp16x8(kong8_16, 7, 92);
		zf_disp4x8(kong8_4, 7, 108); //kong

		zf_disp4x8(None8_4, 7, 112); //-
		zf_disp4x8(None8_4, 7, 116); //-
		zf_disp4x8(None8_4, 7, 120); //-
		zf_disp4x8(None8_4, 7, 124); //-
	}
}

void display_Idle()
{

	LcmClear();
	Update_Page_Header();
	Update_Idle_page4_5_1_2(menu_focus_idx);
}
void display_CeLiang_Page(bit timer_fresh)
{
	unsigned char idx;
	unsigned char offset = 0;

	LcmClear();

	if (celiangPage_idx == CELIANG_WORKING_PAGE)
	{
		if (0 == celiang_mode)
		{
			Update_Page_Header();

			if (signal == 0) //定位不成功.
			{
				searching_sat = 1;
				Display_Chinese(dai, 4, 0); //待
				Display_Chinese(ce, 4, 16); //测
				Display_Chinese(kong, 4, 32); //
				Display_Chinese(zheng, 4, 48);//正
				Display_Chinese(zai, 4, 64); //在
				Display_Chinese(sou, 4, 80); //搜
				Display_Chinese(xing, 4, 96); //星
				Display_Chinese(points, 4, 112); //
			}
			else
			{
				searching_sat = 0;
				Display_Chinese(dai, 4, 0); //待
				Display_Chinese(ce, 4, 16); //测
				Display_Chinese(kong, 4, 32); //
				Display_Chinese(an, 4, 48);//按
				Display_Chinese(que, 4, 64); //确
				Display_Chinese(ren, 4, 80); //认
				Display_Chinese(kai, 4, 96); //开
				Display_Chinese(shi3, 4, 112); //始
			}
		}
		else if (2 == celiang_mode)
		{
			Update_Page_Header();

			Display_Chinese(dan, 4, 0); //单
			Display_Chinese(jia, 4, 16); //价

			for (idx = 0; idx < 4; idx++)
			{
				if (offset > 0)
				{
					zf_disp8x16(Num_8_16[price_per_area[idx]], 4, 32 + offset);
					offset += 8;
				}
				else
				{
					if (price_per_area[idx] != 0)
					{
						zf_disp8x16(Num_8_16[price_per_area[idx]], 4, 32 + offset);
						offset += 8;
					}
					else
					{
						if (idx == 3)
						{
							zf_disp8x16(Num_8_16[price_per_area[idx]], 4, 32 + offset);
							offset += 8;
						}
					}
				}
			}
			Display_Chinese(yuan, 4, 32 + offset); //元
			offset += 16;
			for (; offset < 80; offset += 8)
			{
				zf_disp8x16(kong, 4, 32 + offset);
			}
			Display_Chinese(yi, 4, 80); //已
			Display_Chinese(ji, 4, 96); //记
			Display_Chinese(lu, 4, 112); //录

			if(danwei_sel <2)
			{
				Display_Chinese(mian, 2, 0); //面
				Display_Chinese(ji2, 2, 16); //积
				g_measure_for_show = g_area_value;
			}
			else
			{
				Display_Chinese(zou, 2, 0); //走
				Display_Chinese(chang, 2, 16); //长
				g_measure_for_show = g_length_value;
			}
			// 没有冒号，因为如果单位为公顷的话，字长就不够了
			for (idx = 0; idx < 8; idx++)
			{
				if (g_measure_for_show[idx] != 0x2e)
					zf_disp8x16(Num_8_16[g_measure_for_show[idx] - 0x30], 2, 32 + idx * 8);
				else
					zf_disp8x16(Num_8_16[10], 2, 32 + idx * 8);
			}

			display_danwei(danwei_sel, 2, 96, 0);

			///计算金额部分:
			total_cost = (price_per_area[0] * 1000 + price_per_area[1] * 100 +
						  price_per_area[2] * 10 + price_per_area[3]) * atof(g_measure_for_show);

			if ((total_cost >= 0) && (total_cost <= 99999999))
			{
				sprintf(total_cost_str, "%08.0f", total_cost);

				Display_Chinese(jin, 0, 0); //金
				Display_Chinese(er, 0, 16); //额
				Display_Chinese(maohao, 0, 32); //：

				for (idx = 0; idx < 8; idx++)
				{
					zf_disp8x16(Num_8_16[total_cost_str[idx] - 0x30], 0, 40 + idx * 8);
				}

				Display_Chinese(yuan, 0, 104); //元
				Display_Chinese(kong, 0, 120);
			}
			else
			{
				Display_Chinese(jin, 0, 0); //金
				Display_Chinese(er, 0, 16); //额
				Display_Chinese(maohao, 0, 32); //：
				Display_Chinese(chao, 0, 48);//..
				Display_Chinese(fan, 0, 64); //..
				Display_Chinese(wei3, 0, 80); //..
				Display_Chinese(kong, 0, 96); //..
				Display_Chinese(yuan, 0, 112); //元
			}
		}
		else if (1 == celiang_mode)
		{
			if (gps_first_point == 1)
			{
				Update_Page_Header();

				Display_Chinese(ding, 4, 0); //定
				Display_Chinese(wei, 4, 16); //位
				Display_Chinese(qi, 4, 32); //起
				Display_Chinese(dian, 4, 48);//点
				Display_Chinese(shao, 4, 64); //稍
				Display_Chinese(deng, 4, 80); //等
				Display_Chinese(dengdai, 4, 96); //..
				Display_Chinese(dengdai, 4, 112); //..
			}
			else
			{
				Update_Page_Header();

				Display_Chinese(dan, 4, 0); //单
				Display_Chinese(jia, 4, 16); //价

				for (idx = 0; idx < 4; idx++)
				{
					if (offset > 0)
					{
						zf_disp8x16(Num_8_16[price_per_area[idx]], 4, 32 + offset);
						offset += 8;
					}
					else
					{
						if (price_per_area[idx] != 0)
						{
							zf_disp8x16(Num_8_16[price_per_area[idx]], 4, 32 + offset);
							offset += 8;
						}
						else
						{
							if (idx == 3)
							{
								zf_disp8x16(Num_8_16[price_per_area[idx]], 4, 32 + offset);
								offset += 8;
							}
						}
					}
				}

				Display_Chinese(yuan, 4, 32 + offset); //元
				offset += 16;
				for (; offset < 80; offset += 8)
				{
					zf_disp8x16(kong, 4, 32 + offset);
				}
				if (signal == 0) //定位不成功.
				{
					Display_Chinese(xin, 4, 80); //信
					Display_Chinese(hao, 4, 96); //号
					Display_Chinese(wu, 4, 112); //无
				}
				else if (StarNum <= 2)
				{
					Display_Chinese(xin, 4, 80); //信
					Display_Chinese(hao, 4, 96); //号
					Display_Chinese(cha, 4, 112); //差
				}
				else
				{
					offset = 80;
					if (celiang_run_light >= 1)
					{
						Display_Chinese(ce, 4, 80); //测
						offset += 16;
					}
					if (celiang_run_light >= 2)
					{
						Display_Chinese(liang, 4, 96); //量
						offset += 16;
					}
					if (celiang_run_light >= 3)
					{
						Display_Chinese(zhong, 4, 112); //中
						offset += 16;
					}
					zf_clear_page_to_end(4, offset, 0x00);
					zf_clear_page_to_end(5, offset, 0x00);
					if (timer_fresh)
						celiang_run_light++;
					if (celiang_run_light >= 4)
						celiang_run_light = 0;
				}

				if(danwei_sel<2)
				{
					Display_Chinese(mian, 2, 0); //面
					Display_Chinese(ji2, 2, 16); //积
					g_measure_for_show = g_area_value;
				}
				else
				{
					Display_Chinese(zou, 2, 0); //走
					Display_Chinese(chang, 2, 16); //长
					g_measure_for_show = g_length_value;
				}
				for (idx = 0; idx < 8; idx++)
				{
					if (g_measure_for_show[idx] != 0x2e)
						zf_disp8x16(Num_8_16[g_measure_for_show[idx] - 0x30], 2, 32 + idx * 8);
					else
						zf_disp8x16(Num_8_16[10], 2, 32 + idx * 8);
				}

				display_danwei(danwei_sel, 2, 96, 0);

				total_cost = (price_per_area[0] * 1000 + price_per_area[1] * 100 +
							  price_per_area[2] * 10 + price_per_area[3]) * atof(g_measure_for_show);

				if ((total_cost >= 0) && (total_cost <= 99999999))
				{

					sprintf(total_cost_str, "%08.0f", total_cost);
					Display_Chinese(jin, 0, 0); //金
					Display_Chinese(er, 0, 16); //额
					Display_Chinese(maohao, 0, 32); //：

					for (idx = 0; idx < 8; idx++)
					{
						zf_disp8x16(Num_8_16[total_cost_str[idx]-0x30], 0, 40 + idx * 8);
					}

					Display_Chinese(yuan, 0, 104); //元
					Display_Chinese(kong, 0, 120);
				}
				else
				{
					Display_Chinese(jin, 0, 0); //金
					Display_Chinese(er, 0, 16); //额
					Display_Chinese(maohao, 0, 32); //：
					Display_Chinese(chao, 0, 48);//..
					Display_Chinese(fan, 0, 64); //..
					Display_Chinese(wei3, 0, 80); //..
					Display_Chinese(kong, 0, 96); //..
					Display_Chinese(yuan, 0, 112); //元
				}
			}
		}
	}
	else if (celiangPage_idx == CELIANG_DETAIL_PAGE)
	{
		if (signal == 0) //定位不成功.
		{
			Display_Chinese(ding, 6, 0); //定
			Display_Chinese(wei, 6, 16); //位
			Display_Chinese(zhong, 6, 32); //中
		}
		else
		{
			Display_Chinese(yi, 6, 0);
			Display_Chinese(ding, 6, 16);
			Display_Chinese(wei, 6, 32);
		}

		Display_Chinese(shi, 6, 48); //时
		Display_Chinese(jian, 6, 64); //间
		Display_Chinese(maohao, 6, 80); //：

		zf_disp8x16(Num_8_16[g_beijing_time[0] - 0x30], 6, 88);
		zf_disp8x16(Num_8_16[g_beijing_time[1] - 0x30], 6, 96);
		Display_Chinese(maohao, 6, 104); //：
		zf_disp8x16(Num_8_16[g_beijing_time[2] - 0x30], 6, 112);
		zf_disp8x16(Num_8_16[g_beijing_time[3] - 0x30], 6, 120);

		Display_Chinese(zong, 4, 0); //总
		Display_Chinese(shu, 4, 16); //数
		Display_Chinese(maohao, 4, 32); //：
		zf_disp8x16(Num_8_16[total_sat_LST[0] - 0x30], 4, 40);
		zf_disp8x16(Num_8_16[total_sat_LST[1] - 0x30], 4, 48);
		Display_Chinese(kong, 4, 56); //：
		Display_Chinese(shi2, 4, 64); //使
		Display_Chinese(yong, 4, 80); //用
		Display_Chinese(maohao, 4, 96); //：
		zf_disp8x16(Num_8_16[use_sat_LST[0] - 0x30], 4, 104);
		zf_disp8x16(Num_8_16[use_sat_LST[1] - 0x30], 4, 112);
		Display_Chinese(kong, 4, 120); //：

		Display_Chinese(jing, 2, 0); //经
		Display_Chinese(du, 2, 16); //度
		Display_Chinese(maohao, 2, 32); //:
		zf_disp8x16(Num_8_16[JD[0] - 0x30], 2, 40);
		zf_disp8x16(Num_8_16[JD[1] - 0x30], 2, 48);
		zf_disp8x16(Num_8_16[JD[2] - 0x30], 2, 56);
		zf_disp8x16(Num_8_16[JD[3] - 0x30], 2, 64);
		zf_disp8x16(Num_8_16[JD[4] - 0x30], 2, 72);
		zf_disp8x16(Num_8_16[10], 2, 80);//.
		zf_disp8x16(Num_8_16[JD[6] - 0x30], 2, 88);
		zf_disp8x16(Num_8_16[JD[7] - 0x30], 2, 96);
		zf_disp8x16(Num_8_16[JD[8] - 0x30], 2, 104);
		zf_disp8x16(Num_8_16[JD[9] - 0x30], 2, 112);
		Display_Chinese(kong, 2, 120);

		Display_Chinese(wei2, 0, 0); //1.
		Display_Chinese(du, 0, 16); //1.
		Display_Chinese(maohao, 0, 32); //1.
		zf_disp8x16(Num_8_16[WD[0] - 0x30], 0, 40);
		zf_disp8x16(Num_8_16[WD[1] - 0x30], 0, 48);
		zf_disp8x16(Num_8_16[WD[2] - 0x30], 0, 56);
		zf_disp8x16(Num_8_16[WD[3] - 0x30], 0, 64);
		zf_disp8x16(Num_8_16[10], 0, 72);
		zf_disp8x16(Num_8_16[WD[5] - 0x30], 0, 80);
		zf_disp8x16(Num_8_16[WD[6] - 0x30], 0, 88);
		zf_disp8x16(Num_8_16[WD[7] - 0x30], 0, 96);
		zf_disp8x16(Num_8_16[WD[8] - 0x30], 0, 104);
		Display_Chinese(kong, 0, 112);
	}
	else if (celiangPage_idx == CELIANG_SN_PAGE)
	{
		get_sn_data();
		Display_PD(sn_num_pic);

		zf_disp7x16(Num_7_16[system_data[0]], 1, 33);
		zf_disp7x16(Num_7_16[system_data[1]], 1, 33 + 7 * 1);
		zf_disp7x16(Num_7_16[system_data[2]], 1, 33 + 7 * 2);
		zf_disp7x16(Num_7_16[system_data[3]], 1, 33 + 7 * 3);
		zf_disp7x16(Num_7_16[system_data[4]], 1, 33 + 7 * 4);
		zf_disp7x16(Num_7_16[system_data[5]], 1, 33 + 7 * 5);
		zf_disp7x16(Num_7_16[system_data[6]], 1, 33 + 7 * 6);
		zf_disp7x16(Num_7_16[system_data[7]], 1, 33 + 7 * 7);
		zf_disp7x16(Num_7_16[system_data[8]], 1, 33 + 7 * 8);
		zf_disp7x16(Num_7_16[system_data[9]], 1, 33 + 7 * 9);
		zf_disp7x16(Num_7_16[system_data[10]], 1, 33 + 7 * 10);
		zf_disp7x16(Num_7_16[system_data[11]], 1, 33 + 7 * 11);

		zf_clear_page_to_end(1, 117, 0x00);
		zf_clear_page_to_end(2, 117, 0x00);
	}
}
void display_danjia_Page()
{
	LcmClear();
	Update_danjia_page4_5_1_2(danjia_sel_focus);

}
void display_danwei_Page()
{
	LcmClear();
	Update_danwei_page4_5_1_2(danwei_sel_for_edit);

}
void Update_jilu_page()
{
	unsigned char time[5], str_buff[10], ItoaBuffer[4];
	unsigned char l_data1[10], l_data2[10];
	unsigned char *measure_show;
	unsigned char l_danwei_sel, l_mode;
	unsigned int IndexNewNumBuffer;
	unsigned int danjia, l_danjia;
	unsigned char offset = 0;
	double l_total_cost;
	Get_Data( curr_history_idx, time, &l_mode,
			  l_data1, l_data2, &l_danwei_sel,
			  &l_danjia);
	IndexNewNumBuffer = curr_history_idx + 1;
	sprintf(ItoaBuffer, "%02d", IndexNewNumBuffer);
	LcmClear();
	if (get_history_data_cnt() == 0)
	{
		Display_Chinese(wu, 6, 0);
		Display_Chinese(ji, 6, 16);
		Display_Chinese(lu, 6, 32);
		Display_Chinese(kong, 6, 48);
		Display_Chinese(kong, 6, 64);
		Display_Chinese(kong, 6, 80);
		Display_Chinese(kong, 6, 96);
		Display_Chinese(kong, 6, 112);
	}
	else
	{
		Display_Chinese(di, 6, 0);
		zf_disp8x16(Num_8_16[ItoaBuffer[0] - 0x30], 6, 16);
		zf_disp8x16(Num_8_16[ItoaBuffer[1] - 0x30], 6, 24);
		Display_Chinese(tiao, 6, 32);
		Display_Chinese(kong, 6, 48);
		Display_Chinese(shi, 6, 56);
		Display_Chinese(jian, 6, 72);
		zf_disp8x16(Num_8_16[time[0] - 0x30], 6, 88);
		zf_disp8x16(Num_8_16[time[1] - 0x30], 6, 96);
		Display_Chinese(maohao, 6, 104);
		zf_disp8x16(Num_8_16[time[2] - 0x30], 6, 112);
		zf_disp8x16(Num_8_16[time[3] - 0x30], 6, 120);

		if (1==l_mode)
		{
			Display_Chinese(chang, 4, 0);
			display_num_str_8x16( l_data1, 6, 4, 16 );
			Display_Chinese(kuan, 4, 64);
			display_num_str_8x16( l_data2, 6, 4, 80 );
		}
		else
		{
			Display_Chinese(dan, 4, 0);
			Display_Chinese(jia, 4, 16);
			danjia = l_danjia;
			offset = 32;
			if ((unsigned char) (danjia / 1000) > 0)
			{
				zf_disp8x16(Num_8_16[(unsigned char) (danjia / 1000)], 4, offset);
				offset += 8;
			}
			danjia = danjia % 1000;
			if (offset > 32 || (unsigned char) (danjia / 100) > 0)
			{
				zf_disp8x16(Num_8_16[(unsigned char) (danjia / 100)], 4, offset);
				offset += 8;
			}
			danjia = danjia % 100;
			if (offset > 32 || (unsigned char) (danjia / 10) > 0)
			{
				zf_disp8x16(Num_8_16[(unsigned char) (danjia / 10)], 4, offset);
				offset += 8;
			}
			danjia = danjia % 10;
			zf_disp8x16(Num_8_16[danjia], 4, offset);
			offset += 8;
			Display_Chinese(yuan, 4, offset);
			offset += 16;
			zf_disp8x16(xie_gang, 4, offset);
			offset += 8;

			display_danwei(l_danwei_sel, 4, offset, 0);
		}

		if (1==l_mode)
		{
			l_total_cost = atof(l_data1) * atof(l_data2);
			if (0==l_danwei_sel)
				l_total_cost = l_total_cost/666.666667;
			else
				l_total_cost = l_total_cost/10000.0;
			sprintf(str_buff, "%08.1f", l_total_cost);
			Display_Chinese(mian, 2, 0);
			Display_Chinese(ji2, 2, 16);
			display_num_str_8x16( str_buff, 8, 2, 32 );
			display_danwei(l_danwei_sel, 2, 96, 0);
		}
		else
		{
			if(l_danwei_sel < 2)
			{
				Display_Chinese(mian, 2, 0); //面
				Display_Chinese(ji2, 2, 16); //积
				measure_show = l_data2;
			}
			else
			{
				Display_Chinese(zou, 2, 0); //走
				Display_Chinese(chang, 2, 16); //长
				measure_show = l_data1;
			}
			display_num_str_8x16( measure_show, 8, 2, 32 );
			display_danwei(l_danwei_sel, 2, 96, 0);
		}

		Display_Chinese(jin, 0, 0); //金
		Display_Chinese(er, 0, 16); //额
		if (1==l_mode)
		{
			l_total_cost = l_danjia * l_total_cost;
		}
		else
		{
			l_total_cost = l_danjia * atof(measure_show);
		}
		sprintf(str_buff, "%08.1f", l_total_cost);
		display_num_str_8x16( str_buff, 8, 0, 32 );

		Display_Chinese(yuan, 0, 96); //元
		Display_Chinese(kong, 0, 112); //kong
	}
}
void display_jilu_page()
{
	Update_jilu_page();
}

void dispay_sn_edit_page()
{
	unsigned char i = 0;

	LcmClear();
	if (sn_focus_idx == 20)
		Revers_Data(hao, 6, 0);
	else
		Display_Chinese(hao, 6, 0);

	Display_Chinese(maohao, 6, 16);

	for (i = 0; i < SN_NUM_LEN; i++)
	{
		if (i == sn_focus_idx)
		{
			reverse_zf_disp8x16(Num_8_16[system_data[i]], 6, 32 + 8 * i);
		}
		else
		{
			zf_disp8x16(Num_8_16[system_data[i]], 6, 32 + 8 * i);
		}
	}
	zf_clear_page_to_end(6, 32 + 8 * SN_NUM_LEN, 0x00);
	zf_clear_page_to_end(7, 32 + 8 * SN_NUM_LEN, 0x00);

	if (sn_focus_idx == 30)
		Revers_Data(shi3, 4, 0);
	else
		Display_Chinese(shi3, 4, 0);

	zf_clear_page_to_end(4, 16, 0x00);
	zf_clear_page_to_end(5, 16, 0x00);

	if (sn_focus_idx == 40)
		Revers_Data(chang, 2, 0);
	else
		Display_Chinese(chang, 2, 0);
	Display_Chinese(maohao, 2, 16);
	if (sn_focus_idx == 41)
		reverse_zf_disp8x16(Num_8_16[len_moving[0]], 2, 32);
	else
		zf_disp8x16(Num_8_16[len_moving[0]], 2, 32);

	zf_disp8x16(Num_8_16[10], 2, 40);
	if (sn_focus_idx == 42)
		reverse_zf_disp8x16(Num_8_16[len_moving[1]], 2, 48);
	else
		zf_disp8x16(Num_8_16[len_moving[1]], 2, 48);
	zf_clear_page_to_end(2, 56, 0x00);
	zf_clear_page_to_end(3, 56, 0x00);

	if (sn_focus_idx == 50)
		Revers_Data(shu, 0, 0);
	else
		Display_Chinese(shu, 0, 0);
	Display_Chinese(maohao, 0, 16);
	if (sn_focus_idx == 51)
		reverse_zf_disp8x16(Num_8_16[spa_len], 0, 32);
	else
		zf_disp8x16(Num_8_16[spa_len], 0, 32);
	zf_clear_page_to_end(0, 40, 0x00);
	zf_clear_page_to_end(1, 40, 0x00);
}

void initiate_var(bit p_start)
{
	unsigned char i;
	for (i = 0; i < 20; i++)
	{
		g_area_value[i] = '0';
		g_length_value[i] = '0';
	}

	if (p_start)
	{
		for (i = 0; i < 6; i++)
		{
			g_beijing_time[i] = '0';
		}
		g_beijing_time[i] = '\0';
	}

	for (i = 0; i < 10; i++)
	{
		JD[i] = '0';
	}
	for (i = 0; i < 9; i++)
	{
		WD[i] = '0';
	}
}


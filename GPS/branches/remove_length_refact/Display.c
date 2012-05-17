#include "REG52.h"
#include<intrins.h>
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
    
sbit CS     =  P1^2; 

sbit RES    =  P1^4;

sbit A0     =  P1^3;
sbit LCD_WR =  P3^6;  
sbit LCD_RD =  P3^7;
sbit CHG_F1_STDBY = P1^7;
sbit CHG_F2_CHRG = P3^3;


sbit DB0 =	P0^0 ;
sbit DB1 =	P0^1 ;
sbit DB2 =	P0^2 ;
sbit DB3 =	P0^3 ;
sbit DB4 =	P0^4 ;
sbit DB5 =	P0^5 ;
sbit DB6 =	P0^6 ;
sbit DB7 =	P0^7 ;

sbit CS_LCD =	P1^2 ;
sbit AO =	P1^3 ;
sbit RST=	P1^4 ;

sbit Led_Flash =  P3^5;

sbit d1= P1^0;
sbit d2= P1^1;

unsigned int UTCTimer = 0;
bit FLAG2 = 1;
unsigned int BatQuan = 0;
unsigned int BatQuan_LST;
bit BatQuanF = 0;
extern unsigned char FLAG1;
unsigned char TEST_2;

bit StarNumF = 0; 
bit signal = 0; 
bit TEST4 = 0;
unsigned int TEST_5;
unsigned char TEST_6 = 0;
unsigned int  TEST_7=1;
 unsigned int idata ZouChangValue = 0;
 bit idata  ZouChangValueFlag = 0;
 unsigned int idata mianjiValue = 0;
 bit idata  mianjiValueFlag = 0;
 unsigned int idata JinerValue = 0;
 bit  idata JinerValueFlag = 0;
 unsigned char TEST1 = 0;

 double TEST_8 = 0;
 unsigned char TEST_0[20];

 bit FLAG3 = 0;
 
 unsigned char TEST2[] = {0,0,8,0,0};
 unsigned char TEST3[] = {0,0,0,0,0};
 unsigned char celiang_run_light = 0;

 extern unsigned char danjiasel;
 
extern bit danwei_zouchang_sel;
extern bit danwei_mianji_sel;
extern bit FLAG4;

extern unsigned char TEST_1[5];
extern unsigned char lock; 
extern unsigned char use_sat_LST[3];
extern unsigned char total_sat_LST[3];
extern unsigned char use_sat[3];
extern unsigned char JD[10]; 
extern unsigned char WD[9]; 
extern unsigned char curr_time[7]; 
extern bit celiangPage_detail; 
extern unsigned char celiangPage_idx;
extern unsigned int StarNum; 
extern unsigned char total_sat[3];

extern unsigned char TEST_9[];
extern unsigned char GetLenthValue[];
extern bit gps_first_point;	
extern unsigned char ADCQuaValue;
extern bit GPS_Point_Updata_WD;
extern bit GPS_Point_Updata_JD;
extern bit GPS_Point_Updata_SatNum;

extern bit GPS_Point_Updata_WD_LCD_Fresh;
extern bit GPS_Point_Updata_JD_LCD_Fresh;
extern bit GPS_Point_Updata_SatNum_LCD_Fresh;
extern bit Cacul_GoOn_F;
extern unsigned char system_data[SYSTEM_DATA_SIZE];
extern unsigned char sn_focus_idx;
extern bit searching_sat;

void delay_ms(unsigned int ms)  
{  
 unsigned int n ;  
 while(ms--)
      {  
       n  =  86;  
       while(n--);  
      }  
}  

void LCD_Reset()  
{  
	IE  =  0x00;  
	RES =  0;  
	delay_ms(50);  
	RES =  1;  
	delay_ms(50);  
} 

void Write_Data(uchar dat) 
{

   CS   =  0;   
	 A0   =  1;  
	 data_bus =  dat;
	 LCD_WR   =  0;  
	 LCD_RD   =  1;  
	 CS   =  1;   	 
}


#if 0 
void Write_Data_rail(uchar dat) 
{
   CS   =  1;   
	 A0   =  1;  

	 LCD_WR   =  0;  
	 LCD_RD   =  1; 
	 data_bus =  dat;
	 CS   =  1;   	 
}
#endif

#if 0  
uchar Read_Data(void) 
{
   CS   =  0; 
	 return data_bus ;
	 A0   =  1;  
	 LCD_WR   =  1;  
	 LCD_RD   =  0;  
	 CS   =  1;   	 
}
#endif 

void Write_Instruction(uchar cmd)
{
    CS   =  0;
   	data_bus =  cmd;  
	 A0   =  0;  
	 LCD_WR   =  0;  
	 LCD_RD   =  1;   
	 CS   =  1;  	   
}
void Set_Page_Address(unsigned char page)
{
    page=0xb0|page;
    Write_Instruction(page);
	return;
}
void Set_Column_Address(unsigned char Column)
{	uchar j;
	j=Column;
  Write_Instruction((0x10|(j>>4)));
	Write_Instruction((0x0f&Column));
	return;
}


void LcmClear( void )
{	   uchar i,j;
     Write_Instruction(0x40); 
     for(i=0;i<8;i++)
     {
             Write_Instruction(0xB0|i);
             Write_Instruction(0x10);  
             Write_Instruction(0x00);  
             for(j=0;j<128;j++)
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


void Revers_Data(unsigned char ft[],uchar page,uchar column)
{
		unsigned char k,num;
 
   	Set_Page_Address(page);
    Set_Column_Address(column);
		num=16;
    for(k=0;k<16;k++)
		{
			Write_Data(~ft[num+k]);

		}

		Set_Page_Address(page+1);
    Set_Column_Address(column);
		num=0;
    for(k=0;k<16;k++)
		{

			Write_Data(~ft[num+k]);
		}


}


void Initial_lcd(void)
{	 
	 CS  =  0;   
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



 void Display_Chinese(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;
   		Set_Page_Address(page);
      Set_Column_Address(column);
		num=16;
    for(k=0;k<16;k++)
		{		 
			Write_Data(ft[num+k]);
		}

		Set_Page_Address(page+1);
    Set_Column_Address(column);
		num=0;
   	for(k=0;k<16;k++)
		{	 
			Write_Data(ft[num+k]);
		}

    return;	 
}
void Display(unsigned char *p) 
{  
 unsigned char page,column;  
 for(page=0xB7;page>=0xB0;page--)  
    {  
     Write_Instruction(page);  
     Write_Instruction(0x10);  
     Write_Instruction(0x00);  
     for(column=0;column<128;column++)  
        {  
         Write_Data(*p++);  
        } 
		delay_ms(7000); 
    } 
	return;
} 
void Display_PD(unsigned char *p) 
{  
 unsigned char page,column;  
 for(page=0xB7;page>=0xB0;page--)  
    {  
     Write_Instruction(page); 
     Write_Instruction(0x10); 
     Write_Instruction(0x00); 
     for(column=0;column<128;column++)  
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

void zf_disp8x16(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;
 		Set_Page_Address(page);
    Set_Column_Address(column);
		num=8;
        for(k=0;k<8;k++)
		{
      		Write_Data(ft[num+k]);
		}

		Set_Page_Address(page+1);
    Set_Column_Address(column);
		num=0;
        for(k=0;k<8;k++)
		{
      		Write_Data(ft[num+k]);
		}
		
}

void zf_disp7x16(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;
 		Set_Page_Address(page);
    Set_Column_Address(column);
		num=7;
        for(k=0;k<7;k++)
		{
      		Write_Data(ft[num+k]);
		}

		Set_Page_Address(page+1);
    Set_Column_Address(column);
		num=0;
        for(k=0;k<7;k++)
		{
      		Write_Data(ft[num+k]);
		}

}


void reverse_zf_disp8x16(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;
 		Set_Page_Address(page);
    Set_Column_Address(column);
		num=8;
        for(k=0;k<8;k++)
		{
      		Write_Data(~ft[num+k]);
		}

		Set_Page_Address(page+1);
    Set_Column_Address(column);
		num=0;
        for(k=0;k<8;k++)
		{
      		Write_Data(~ft[num+k]);
		}
		
}


void zf_clear_page_to_end(uchar page, uchar column)
{
	unsigned char k;
	Set_Page_Address(page);
	Set_Column_Address(column);
	for(k=column;k<128;k++)
	{
		Write_Data(0x00);
	}
}

void zf_disp8x8(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;
	num=0;
	Set_Page_Address(page);
    Set_Column_Address(column);
    for(k=0;k<8;k++)
	{
  		Write_Data(ft[num+k]);
	}		
}


void zf_disp16x8(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;;
	num=0;
	Set_Page_Address(page);
    Set_Column_Address(column);
    for(k=0;k<16;k++)
	{
  		Write_Data(ft[num+k]);
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

void zf_disp4x8(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;;
	num=0;
	Set_Page_Address(page);
  Set_Column_Address(column);
  for(k=0;k<4;k++)
	{
  		Write_Data(ft[num+k]);
	}		
}


 void Display_xinhao(unsigned char ft[],uchar page,uchar column)
{
	unsigned char k,num=0;
   		Set_Page_Address(page);
      Set_Column_Address(column);
		num=0;
    for(k=0;k<16;k++)
		{
      		Write_Data(ft[num+k]);
		}
		Set_Page_Address(page+1);
    Set_Column_Address(column);
		num=16;
        for(k=0;k<16;k++)
		{
      		Write_Data(ft[num+k]);
		}
    return;
}


void display_chinesestring()
{

	Display(logo);  
    delay_ms(3000);

 
	LcmClear();

	Display_Chinese(num3,1,0); 
	Display_Chinese(dan,1,16); 
	Display_Chinese(wei,1,32); 
	Display_Chinese(kong,1,48); 

	Display_Chinese(num4,1,64);
	Display_Chinese(ji,1,80); 
	Display_Chinese(lu,1,96); 

	Display_Chinese(kong,1,112);
	Display_Chinese(kong,1,128);


	

 	
	Display_Chinese(num1,4,0); 
	Display_Chinese(ce,4,16);
	Display_Chinese(liang,4,32);

	Display_Chinese(kong,4,48);

	Display_Chinese(num2,4,64); 
	Display_Chinese(dan,4,80); 
	Display_Chinese(jia,4,96); 

	Display_Chinese(kong,4,112);
	Display_Chinese(kong,4,128);

	zf_disp4x8(num[1],7,4); //1
	zf_disp4x8(num[2],7,8); //1
	zf_disp4x8(num[10],7,12); //：
	zf_disp4x8(num[3],7,16); //3
	zf_disp4x8(num[0],7,20); //0
	Display_Chinese(kong,7,24);
	Display_Chinese(kong,7,39);
	Display_Chinese(kong,7,55);
	Display_Chinese(kong,7,71);
		Display_Chinese(kong,7,87);
			Display_Chinese(kong,7,103);
	zf_disp16x8(xinhao,7,110);


	delay_ms(10000);
	
	
	Revers_Data(num3,1,0); 
	Revers_Data(dan,1,16); 
	Revers_Data(wei,1,32); 
	}
void display_LOGO()
{

	LcmClear();
	Display(logo);  
    delay_ms(6000);	
	TEST1 = 0;
	
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

void Update_Idle_page4_5_1_2(unsigned char Sel_flag)
{
   			Display_Chinese(num1,4,0); 
			Display_Chinese(ce,4,16); 
			Display_Chinese(liang,4,32); 
			Display_Chinese(kong,4,48);	
			Display_Chinese(num2,4,64); 
			Display_Chinese(dan,4,80); 
			Display_Chinese(jia,4,96); 
			Display_Chinese(kong,4,112);
			Display_Chinese(num3,1,0); 
			Display_Chinese(dan,1,16); 
			Display_Chinese(wei,1,32); 
			Display_Chinese(kong,1,48);	
			Display_Chinese(num4,1,64);
			Display_Chinese(ji,1,80); 
			Display_Chinese(lu,1,96); 
			Display_Chinese(kong,1,112);
	if((Sel_flag == 0)||(Sel_flag == 1))
	{
		if(Sel_flag == 0)
		{
				Revers_Data(num1,4,0);
				Revers_Data(ce,4,16); 
				Revers_Data(liang,4,32); 
				Display_Chinese(kong,4,48);	
				Display_Chinese(num2,4,64);
				Display_Chinese(dan,4,80); 
				Display_Chinese(jia,4,96); 
				Display_Chinese(kong,4,112);
		}
		else
		{
   				Display_Chinese(num1,4,0); 
				Display_Chinese(ce,4,16); 
				Display_Chinese(liang,4,32);
				Display_Chinese(kong,4,48);	
				Revers_Data(num2,4,64); 
				Revers_Data(dan,4,80); 
				Revers_Data(jia,4,96); 
				Display_Chinese(kong,4,112);
		}
	}
	else if((Sel_flag == 2)||(Sel_flag == 3))
	{
		if(Sel_flag == 2)
		{
				Revers_Data(num3,1,0); 
				Revers_Data(dan,1,16); 
				Revers_Data(wei,1,32); 
				Display_Chinese(kong,1,48);	
				Display_Chinese(num4,1,64);
				Display_Chinese(ji,1,80);
				Display_Chinese(lu,1,96);
				Display_Chinese(kong,1,112);
		}
		else
		{
   				Display_Chinese(num3,1,0); 
				Display_Chinese(dan,1,16); 
				Display_Chinese(wei,1,32); 
				Display_Chinese(kong,1,48);	
				Revers_Data(num4,1,64); 
				Revers_Data(ji,1,80); 
				Revers_Data(lu,1,96); 
				Display_Chinese(kong,1,112);	
		}
	}


}
void Update_danjia_page4_5_1_2(unsigned char Sel_flag)	  
{
			if(Sel_flag == 0)
			{
			Revers_Data(mian,4,0);
			Revers_Data(ji2,4,16); 
			}
			else
			{
   			Display_Chinese(mian,4,0); 
			Display_Chinese(ji2,4,16); 
				
			}
			Display_Chinese(maohao,4,32); 
			zf_disp8x16(kong,4,48);
			/*
			if(Sel_flag == 1)
			reverse_zf_disp8x16(Num_8_16[TEST2[0]], 4, 48);
			else
			zf_disp8x16(Num_8_16[TEST2[0]], 4, 48);	
			*/
			if(Sel_flag == 2)
			reverse_zf_disp8x16(Num_8_16[TEST2[1]], 4, 56);
			else
			zf_disp8x16(Num_8_16[TEST2[1]], 4, 56);	

			if(Sel_flag == 3)
			reverse_zf_disp8x16(Num_8_16[TEST2[2]], 4, 64);
			else
			zf_disp8x16(Num_8_16[TEST2[2]], 4, 64);

			if(Sel_flag == 4)
			reverse_zf_disp8x16(Num_8_16[TEST2[3]], 4, 72);
			else
			zf_disp8x16(Num_8_16[TEST2[3]], 4, 72);	

			Display_Chinese(kong,4,80);
			Display_Chinese(yuan,4,96);
			Display_Chinese(kong,4,112);
/*
			if(Sel_flag == 5)
			{
			Revers_Data(chang,1,0); 
			Revers_Data(du,1,16); 
			}
			else
			{
   			Display_Chinese(chang,1,0);
			Display_Chinese(du,1,16); 
			}

   			
			Display_Chinese(maohao,1,32);
			if(Sel_flag == 6)
			reverse_zf_disp8x16(Num_8_16[TEST3[0]], 1, 48);
			else
			zf_disp8x16(Num_8_16[TEST3[0]], 1, 48);

			if(Sel_flag == 7)
			reverse_zf_disp8x16(Num_8_16[TEST3[1]], 1, 56);
			else
			zf_disp8x16(Num_8_16[TEST3[1]], 1, 56);	

			if(Sel_flag == 8)
			reverse_zf_disp8x16(Num_8_16[TEST3[2]], 1, 64);
			else
			zf_disp8x16(Num_8_16[TEST3[2]], 1, 64);

			if(Sel_flag == 9)
			reverse_zf_disp8x16(Num_8_16[TEST3[3]], 1, 72);
			else
			zf_disp8x16(Num_8_16[TEST3[3]], 1, 72);	
		   	Display_Chinese(kong,1,80);
			

			
			Display_Chinese(yuan,1,96);
			Display_Chinese(kong,1,112);
*/

}


void Update_danwei_page4_5_1_2(bit Sel_zouchang_flag,bit Sel_mianji_flag,bit sel_ZCMJ_SEL)	  
{
	Display_Chinese(mian,4,0); //面
	Display_Chinese(ji2,4,16); //积

	Display_Chinese(maohao,4,32); //:
	if(Sel_mianji_flag == 0)
	{
		Revers_Data(mu,4,48);//亩
		Display_Chinese(kong,4,64);
		Display_Chinese(gong,4,80); //公
		Display_Chinese(qing,4,96); //倾
	}
	else
	{
		Display_Chinese(mu,4,48); //亩
		Display_Chinese(kong,4,64);
		Revers_Data(gong,4,80);//公
		Revers_Data(qing,4,96);//倾
	}

	Display_Chinese(kong,4,112);

#if 0
			if(sel_ZCMJ_SEL == 0)
			{
			Revers_Data(zou,4,0);//米
		    Revers_Data(chang,4,16); //长
			}
			else
			{
   			Display_Chinese(zou,4,0); //走
			Display_Chinese(chang,4,16); //长
			}
				
			Display_Chinese(maohao,4,32); //:		

			if(Sel_zouchang_flag == 0)
			{
				Revers_Data(mi,4,48);//米
				Display_Chinese(kong,4,64);
				Display_Chinese(qian,4,80); //千
				Display_Chinese(mi,4,96); //米	
			}
			else
			{
				Display_Chinese(mi,4,48); //米
				Display_Chinese(kong,4,64);
				Revers_Data(qian,4,80);//千
				Revers_Data(mi,4,96);//米
			}
		
			Display_Chinese(kong,4,112);


			if(sel_ZCMJ_SEL == 1)
			{
			Revers_Data(mian,1,0);
		    Revers_Data(ji2,1,16);
			}
			else
			{
   			Display_Chinese(mian,1,0); //面
			Display_Chinese(ji2,1,16); //积	
			}
				
			Display_Chinese(maohao,1,32); //:
			if(Sel_mianji_flag == 0)
			{
			Revers_Data(mu,1,48);//亩
			Display_Chinese(kong,1,64);
			Display_Chinese(gong,1,80); //公
			Display_Chinese(qing,1,96); //倾	
			}
			else
			{
			Display_Chinese(mu,1,48); //亩
			Display_Chinese(kong,1,64);
			Revers_Data(gong,1,80);//公
			Revers_Data(qing,1,96);//倾	
			}

			
			Display_Chinese(kong,1,112);
#endif
}

void Update_Idle_Page7(unsigned int Timer_L,unsigned int BatQuan_L,unsigned int StarNum_L,bit signal_L)
{
unsigned int minu0,minu1,hour0,hour1,StarNum0,StarNum1,startcol;
unsigned char test[6];
if((FLAG2)||(BatQuanF)||(StarNumF)||(signal))
{

minu0 = Timer_L%10;		
Timer_L = Timer_L/10;	
minu1 = Timer_L%10;		
Timer_L = Timer_L/10;	
hour0 = Timer_L%10;		
Timer_L = Timer_L/10;	
hour1 = Timer_L%10;		


StarNum0 = StarNum_L%10;
StarNum_L = StarNum_L/10;
StarNum1 = StarNum_L%10;

startcol = 0;
		if((CHG_F1_STDBY == 0))
		{
			if(BatQuan >= 4 )
			{
			
			ADCQuaValue = GetADCResult(0);
			ADC2BATVALUE();
			if((BatQuan >= 4)&&(TEST_7))
				{
					BatQuan = 3;
					TEST_7++;
					if(TEST_7>=1000)
					{
						TEST_7 =0;
						BatQuan=4;
					}
	
				}

			}
			else
			BatQuan++;
		}
		else
		{
			ADCQuaValue = GetADCResult(0);
			ADC2BATVALUE();
			TEST_7 =1;
		}


if((lock != 0x30)||((use_sat[0]-0x30)*10+ (use_sat[1]-0x30)))
{
	
  	zf_disp4x8(num[hour1],7,startcol); //小时1
	zf_disp4x8(num[hour0],7,startcol+4); //小时2
	zf_disp4x8(num[10],7,startcol+8); //：
	zf_disp4x8(num[minu1],7,startcol+12); //分钟1
	zf_disp4x8(num[minu0],7,startcol+16); //分钟2

	
	zf_disp8x8(kong8_8,7,startcol+20);
	zf_disp16x8(kong8_16,7,startcol+28);
	
	zf_disp16x8(font5[BatQuan_L],7,startcol+44); //电池
	zf_disp8x8(kong8_8,7,startcol+60);
	
	if((1 == Cacul_GoOn_F)&&(1==FLAG1))
	{
	zf_disp8x8(zanting8_8,7,startcol+68);
	zf_disp8x8(kong8_8,7,startcol+76);
	}
	else	
	zf_disp16x8(kong8_16,7,startcol+68);
	
	zf_disp4x8(num[StarNum1],7,startcol+84); 
	zf_disp4x8(num[StarNum0],7,startcol+88); //星数量
	
	zf_disp16x8(kong8_16,7,startcol+92);
		zf_disp4x8(kong8_4,7,startcol+108); //kong
	zf_disp16x8(xinhao,7,startcol+112); //信号
	//}


}
else
{

	zf_disp4x8(None8_4,7,startcol); //小时1
	zf_disp4x8(None8_4,7,startcol+4); //小时2
	zf_disp4x8(num[10],7,startcol+8); //：
	zf_disp4x8(None8_4,7,startcol+12); //分钟1
	zf_disp4x8(None8_4,7,startcol+16); //横线

	zf_disp8x8(kong8_8,7,startcol+20);
	zf_disp16x8(kong8_16,7,startcol+28);

	zf_disp16x8(font5[BatQuan_L],7,startcol+44); //电池

	zf_disp8x8(kong8_8,7,startcol+60);
	zf_disp16x8(kong8_16,7,startcol+68);

	zf_disp4x8(None8_4,7,startcol+84); //星数量
	zf_disp4x8(None8_4,7,startcol+88); //星数量

	zf_disp16x8(kong8_16,7,startcol+92);

	zf_disp4x8(kong8_4,7,startcol+108); //kong

	zf_disp4x8(None8_4,7,startcol+112); //星数量
	zf_disp4x8(None8_4,7,startcol+116); //星数量
	zf_disp4x8(None8_4,7,startcol+120); //星数量
	zf_disp4x8(None8_4,7,startcol+124); //星数量



}
}
}

void display_Idle()
{

	LcmClear();
	UTC2BeiJingTime();
	TEST_5 = (TEST_1[0]-0x30)*1000+(TEST_1[1]-0x30)*100+(TEST_1[2]-0x30)*10+(TEST_1[3]-0x30);
	 Update_Idle_Page7(TEST_5,BatQuan,StarNum,signal);
	 Update_Idle_page4_5_1_2(TEST_6);	
}
void display_CeLiang_Page( bit timer_fresh)
{
	unsigned int StartCol= 40;
	unsigned char celiangCount;
	unsigned char CharBuffer[6];
	unsigned char offset = 0;
	
		if(celiangPage_idx==CELIANG_WORKING_PAGE)
		{
			LcmClear();
			if(0 == FLAG1)
			{
				TEST_5 = (TEST_1[0]-0x30)*1000+(TEST_1[1]-0x30)*100+(TEST_1[2]-0x30)*10+(TEST_1[3]-0x30);
				Update_Idle_Page7(TEST_5,BatQuan,StarNum,signal);

				if((lock == 0x30) && (((use_sat[0]-0x30)*10+ (use_sat[1]-0x30))==0))	//定位不成功.
				{
					searching_sat = 1;
					Display_Chinese(dai,4,0); //待
					Display_Chinese(ce,4,16); //测
					Display_Chinese(kong,4,32); //
					Display_Chinese(zheng,4,48);//正
					Display_Chinese(zai,4,64); //在
					Display_Chinese(sou,4,80); //搜
					Display_Chinese(xing,4,96); //星
					Display_Chinese(points,4,112); //
				}
				else
				{
					searching_sat = 0;
					Display_Chinese(dai,4,0); //待
					Display_Chinese(ce,4,16); //测
					Display_Chinese(kong,4,32); //
					Display_Chinese(an,4,48);//按
					Display_Chinese(que,4,64); //确
					Display_Chinese(ren,4,80); //认
					Display_Chinese(kai,4,96); //开
					Display_Chinese(shi3,4,112); //始
				}
			}
			else if(2 == FLAG1)
			{
			   TEST_5 = (TEST_1[0]-0x30)*1000+(TEST_1[1]-0x30)*100+(TEST_1[2]-0x30)*10+(TEST_1[3]-0x30);
			   Update_Idle_Page7(TEST_5,BatQuan,StarNum,signal);

			   Display_Chinese(dan,4,0); //单
			   Display_Chinese(jia,4,16); //价

			   for(celiangCount=0;celiangCount<4;celiangCount++)
			   {
				   if( offset>0 )
				   {
					   zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+offset);
					   offset += 8;
				   }
				   else
				   {
					   if(TEST2[celiangCount] != 0)
					   {
						   zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+offset);
						   offset += 8;
					   }
					   else
					   {
						   if(celiangCount == 3)
						   {
							   zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+offset);
							   offset += 8;
						   }
					   }
				   }
				   /*
				   if(TEST2[celiangCount] != 0x2e)
					   zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+(celiangCount*8));
				   else
					   zf_disp8x16(Num_8_16[10], 4, 32+celiangCount*8);
				   */
			   }
			   Display_Chinese(yuan,4,32+offset); //元
			   offset += 16;
			   for( ; offset<80; offset+=8)
			   {
				   zf_disp8x16(kong, 4, 32+offset);
			   }
			   Display_Chinese(yi,4,80); //已
			   Display_Chinese(ji,4,96); //记
			   Display_Chinese(lu,4,112); //录

			   Display_Chinese(mian,2,0); //面
			   Display_Chinese(ji2,2,16); //积
			   //Display_Chinese(maohao,2,32); //：
			   for(celiangCount=0;celiangCount<8;celiangCount++)
			   {
				   if(TEST_9[celiangCount] != 0x2e)
					   zf_disp8x16(Num_8_16[TEST_9[celiangCount]-0x30], 2, 32+celiangCount*8);
				   else
					   zf_disp8x16(Num_8_16[10], 2, 32+celiangCount*8);
			   }

			   if(0 == danwei_mianji_sel)
			   {
				   Display_Chinese(mu,2,96); //亩
				   Display_Chinese(kong,2,112);
			   }
			   else
			   {
				   Display_Chinese(gong,2,96); //公
				   Display_Chinese(qing,2,112);//倾
			   }

			   ///计算金额部分:
			   if((danjiasel >= 0)&&(danjiasel <= 4))//面积
			   {
				   TEST_8 = (TEST2[0]*1000+TEST2[1]*100+TEST2[2]*10+TEST2[3])*atof(TEST_9);
			   }
			   else  //长度
			   {
				   TEST_8 = (TEST3[0]*1000+TEST3[1]*100+TEST3[2]*10+TEST3[3])*atof(GetLenthValue);
			   }

			   if((TEST_8 >= 0)&&(TEST_8 <= 99999999))
			   {
				   sprintf(TEST_0, "%08.0f", TEST_8);
				   //					DataSend("\r\n TEST_0:");
				   //					DataSend(TEST_0);

				   Display_Chinese(jin,0,0); //金
				   Display_Chinese(er,0,16); //额
				   Display_Chinese(maohao,0,32); //：

				   for(celiangCount=0;celiangCount<8;celiangCount++)
				   {
					   zf_disp8x16(Num_8_16[TEST_0[celiangCount]-0x30], 0, 40+celiangCount*8);
				   }

				   Display_Chinese(yuan,0,104); //元
				   Display_Chinese(kong,0,120);
			   }
			   else
			   {
			   				   Display_Chinese(jin,0,0); //金
			   				   Display_Chinese(er,0,16); //额
			   				   Display_Chinese(maohao,0,32); //：
			   				   Display_Chinese(chao,0,48);//..
			   				   Display_Chinese(fan,0,64); //..
			   				   Display_Chinese(wei3,0,80); //..
			   				   Display_Chinese(kong,0,96); //..
			   				   Display_Chinese(yuan,0,112); //元
			   }
			}
			else if(1 == FLAG1)
			{
				if(gps_first_point == 1)
				{
						TEST_5 = (TEST_1[0]-0x30)*1000+(TEST_1[1]-0x30)*100+(TEST_1[2]-0x30)*10+(TEST_1[3]-0x30);
						Update_Idle_Page7(TEST_5,BatQuan,StarNum,signal);
						Display_Chinese(ding,4,0); //定
						Display_Chinese(wei,4,16); //位
						Display_Chinese(qi,4,32); //起
						Display_Chinese(dian,4,48);//点
						Display_Chinese(shao,4,64); //稍
						Display_Chinese(deng,4,80); //等
						Display_Chinese(dengdai,4,96); //..
						Display_Chinese(dengdai,4,112); //..
					//	Display_Chinese(dengdai,4,120);
			
				}
	
				else
				{
					TEST_5 = (TEST_1[0]-0x30)*1000+(TEST_1[1]-0x30)*100+(TEST_1[2]-0x30)*10+(TEST_1[3]-0x30);
					Update_Idle_Page7(TEST_5,BatQuan,StarNum,signal);

					Display_Chinese(dan,4,0); //单
					Display_Chinese(jia,4,16); //价
				//	Display_Chinese(maohao,4,32); //：
			
				//	zf_disp8x16(Num_8_16[TEST_1[0]-0x30], 6, 88);
					for(celiangCount=0;celiangCount<4;celiangCount++)
					{
						if( offset>0 )
						{
							zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+offset);
							offset += 8;
						}
						else
						{
							if(TEST2[celiangCount] != 0)
							{
								zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+offset);
								offset += 8;
							}
							else
							{
								if(celiangCount == 3)
								{
									zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+offset);
									offset += 8;
								}
							}
						}
						/*
						if(TEST2[celiangCount] != 0x2e)
						zf_disp8x16(Num_8_16[TEST2[celiangCount]], 4, 32+(celiangCount*8));
						else
						zf_disp8x16(Num_8_16[10], 4, 32+celiangCount*8);
						*/
					}

					Display_Chinese(yuan,4,32+offset); //元
					offset += 16;
					for( ; offset<80; offset+=8)
					{
						zf_disp8x16(kong, 4, 32+offset);
					}
					if((lock == 0x30) && (((use_sat[0]-0x30)*10+ (use_sat[1]-0x30))==0))	//定位不成功.
					{
					   Display_Chinese(xin,4,80); //信
					   Display_Chinese(hao,4,96); //号
					   Display_Chinese(wu,4,112); //无
					}
					else if(((use_sat[0]-0x30)*10+ (use_sat[1]-0x30))<=2)
					{
						Display_Chinese(xin,4,80); //信
						Display_Chinese(hao,4,96); //号
						Display_Chinese(cha,4,112); //差
					}
					else
					{
						offset = 80;
						if(celiang_run_light>=1)
						{
							Display_Chinese(ce,4,80); //测
							offset += 16;
						}
						if(celiang_run_light>=2)
						{
							Display_Chinese(liang,4,96); //量
							offset += 16;
						}
						if(celiang_run_light>=3)
						{
							Display_Chinese(zhong,4,112); //中
							offset += 16;
						}
						zf_clear_page_to_end(4,offset);
						zf_clear_page_to_end(5,offset);
						if(timer_fresh)
							celiang_run_light++;
						if(celiang_run_light >= 4)
							celiang_run_light = 0;

					}
					//	zf_disp8x16(Num_8_16[GetLenthValue[5]-0x30], 4, 40);
					/*
					if(0 == danwei_zouchang_sel) //米
					{
					Display_Chinese(mi,4,96); //米
					Display_Chinese(kong,4,112);
					}
					else
					{
					 Display_Chinese(qian,4,96); //千
					Display_Chinese(mi,4,112);	//米
					}
					*/

			
					Display_Chinese(mian,2,0); //面
					Display_Chinese(ji2,2,16); //积
					//Display_Chinese(maohao,2,32); //：
					for(celiangCount=0;celiangCount<8;celiangCount++)
					{
						if(TEST_9[celiangCount] != 0x2e)
						zf_disp8x16(Num_8_16[TEST_9[celiangCount]-0x30], 2, 32+celiangCount*8);	
						else
						zf_disp8x16(Num_8_16[10], 2, 32+celiangCount*8);	
					}
				//	Display_Chinese(dengdai,2,40);//..	
				//	Display_Chinese(dengdai,2,56); //..
				//	Display_Chinese(dengdai,2,72); //..
				//	Display_Chinese(dengdai,2,88); //..
				//	Display_Chinese(dengdai,2,104); //..
					if(0 == danwei_mianji_sel)
					{
					Display_Chinese(mu,2,96); //亩
					Display_Chinese(kong,2,112);
					}
					else
					{
					Display_Chinese(gong,2,96); //公
					Display_Chinese(qing,2,112);//倾
					}
					
					
					
					///计算金额部分:
					if((danjiasel >= 0)&&(danjiasel <= 4))//面积
					{
					//TEST_8 = ((TEST2[0]-0x30)*1000 + (TEST2[1]-0x30)*100 + (TEST2[2]-0x30)*10 +(TEST2[3]-0x30))*atof(TEST_9);
				//	memcpy(CharBuffer,TEST2,4);
				//	TEST2[4]='\0';
					TEST_8 = (TEST2[0]*1000+TEST2[1]*100+TEST2[2]*10+TEST2[3])*atof(TEST_9);
				//	DataSend("\r\ndanjiasel == 0~4");
					
					}
					else  //长度
					{
				//	 memcpy(CharBuffer,TEST3,4);
				//	TEST3[4]=0;
					TEST_8 = (TEST3[0]*1000+TEST3[1]*100+TEST3[2]*10+TEST3[3])*atof(GetLenthValue);
				//	DataSend("\r\nTEST3:");
				//	DataSend(TEST3);
				//	DataSend("\r\ngetlenthvalue:");
				//	DataSend(GetLenthValue);
					}
					
	
					if((TEST_8 >= 0)&&(TEST_8 <= 99999999))
					{
				
					sprintf(TEST_0, "%08.0f", TEST_8);
	//					DataSend("\r\n TEST_0:");
	//					DataSend(TEST_0);
					Display_Chinese(jin,0,0); //金
					Display_Chinese(er,0,16); //额
					Display_Chinese(maohao,0,32); //：
	
					for(celiangCount=0;celiangCount<8;celiangCount++)
					{
					//	if(TEST_0[celiangCount] != 0x2e)
						zf_disp8x16(Num_8_16[TEST_0[celiangCount]-0x30], 0, 40+celiangCount*8);	
					//	else
					//	zf_disp8x16(Num_8_16[10], 0, 40+celiangCount*8);	
					}
					/*
					
					Display_Chinese(dengdai,0,48);//..	
					Display_Chinese(dengdai,0,64); //..
					Display_Chinese(dengdai,0,80); //..
					Display_Chinese(dengdai,0,96); //..*/
					Display_Chinese(yuan,0,104); //元
					Display_Chinese(kong,0,120);
					//Display_Chinese(yuan,0,112); //元
					//Display_Chinese(kong,0,112);
					}
					else
					{
					Display_Chinese(jin,0,0); //金
					Display_Chinese(er,0,16); //额
					Display_Chinese(maohao,0,32); //：
					Display_Chinese(chao,0,48);//..	
					Display_Chinese(fan,0,64); //..
					Display_Chinese(wei3,0,80); //..
					Display_Chinese(kong,0,96); //..
					Display_Chinese(yuan,0,112); //元	
					}
	//				DataSend("\r\n--TEST_8:");
	//				DataSend(TEST_0);
				}
			}
		}
		else if(celiangPage_idx==CELIANG_DETAIL_PAGE)
		{
		/*
			Display_Chinese(ding,4,0); //1.
			Display_Chinese(wei,4,16); //测
			Display_Chinese(zhong,4,32); //量
			Display_Chinese(dengdai,4,48);//..	
			Display_Chinese(dengdai,4,64); //..
			Display_Chinese(kong,4,80); //空
			Display_Chinese(kong,4,96); //空
			Display_Chinese(kong,4,112);
		
			zf_disp4x8(num[TEST_1[0]-0x30],7,StartCol+0); //小时1	TEST_1
			zf_disp4x8(num[TEST_1[1]-0x30],7,StartCol+4); //小时1	TEST_1
			zf_disp4x8(num[10],7,StartCol+8); //：
			zf_disp4x8(num[TEST_1[2]-0x30],7,StartCol+12); //小时1	TEST_1
			zf_disp4x8(num[TEST_1[3]-0x30],7,StartCol+16); //小时1	TEST_1
			
		*/	
			  //if((GPS_Point_Updata_WD_LCD_Fresh == 1)&&((GPS_Point_Updata_JD_LCD_Fresh == 1))&&(GPS_Point_Updata_SatNum_LCD_Fresh == 1))
			 // {
			  	
			  	LcmClear();
				

				if((lock == 0x30)||(((use_sat[0]-0x30)*10+ (use_sat[1]-0x30))==0))	//定位不成功.
				{
				Display_Chinese(ding,6,0); //定
				Display_Chinese(wei,6,16); //位
				Display_Chinese(zhong,6,32); //中
		
				//zf_disp8x16(Num_8_16[curr_time[0]-0x30], 6, 48);
				//zf_disp8x16(Num_8_16[curr_time[1]-0x30], 6, 56);
				//zf_disp8x16(Num_8_16[curr_time[2]-0x30], 6, 64);
				//zf_disp8x16(Num_8_16[curr_time[3]-0x30], 6, 72);
				//zf_disp8x16(Num_8_16[curr_time[4]-0x30], 6, 80);
				//zf_disp8x16(Num_8_16[curr_time[5]-0x30], 6, 88);
				//zf_disp8x16(Num_8_16[curr_time[6]-0x30], 6, 96);
		
				
				}
				else
				
				{
				Display_Chinese(yi,6,0);
				Display_Chinese(ding,6,16);
				Display_Chinese(wei,6,32);
				}
			
				Display_Chinese(shi,6,48); //时
			   	Display_Chinese(jian,6,64); //间
				Display_Chinese(maohao,6,80); //：
				
				zf_disp8x16(Num_8_16[TEST_1[0]-0x30], 6, 88);
				zf_disp8x16(Num_8_16[TEST_1[1]-0x30], 6, 96);
				Display_Chinese(maohao,6,104); //：
			//	zf_disp8x16(Num_8_16[10], 6, 104);
				zf_disp8x16(Num_8_16[TEST_1[2]-0x30], 6, 112);
				zf_disp8x16(Num_8_16[TEST_1[3]-0x30], 6, 120);
		
		
				
				Display_Chinese(zong,4,0); //1.
				Display_Chinese(shu,4,16); //1.
				Display_Chinese(maohao,4,32); //：
				zf_disp8x16(Num_8_16[total_sat_LST[0]-0x30], 4, 40);
				zf_disp8x16(Num_8_16[total_sat_LST[1]-0x30], 4, 48);
				
				Display_Chinese(kong,4,56); //：

		
				{

				GPS_Point_Updata_SatNum_LCD_Fresh = 0;
				GPS_Point_Updata_WD_LCD_Fresh = 0;
				GPS_Point_Updata_JD_LCD_Fresh = 0;

				Display_Chinese(shi2,4,64); //1.
				Display_Chinese(yong,4,80); //1.
				Display_Chinese(maohao,4,96); //：
				zf_disp8x16(Num_8_16[use_sat_LST[0]-0x30], 4, 104);
				zf_disp8x16(Num_8_16[use_sat_LST[1]-0x30], 4, 112);	 
				Display_Chinese(kong,4,120); //：  
		
				Display_Chinese(jing,2,0); //1.
				Display_Chinese(du,2,16); //1.
				Display_Chinese(maohao,2,32); //1.
				zf_disp8x16(Num_8_16[JD[0]-0x30], 2, StartCol+0);
				zf_disp8x16(Num_8_16[JD[1]-0x30], 2, StartCol+8);
				zf_disp8x16(Num_8_16[JD[2]-0x30], 2, StartCol+16);
				zf_disp8x16(Num_8_16[JD[3]-0x30], 2, StartCol+24);
				zf_disp8x16(Num_8_16[JD[4]-0x30], 2, StartCol+32);
				zf_disp8x16(Num_8_16[10], 2, StartCol+40);
				//Display_Chinese(maohao,2,StartCol+40); //1.
				zf_disp8x16(Num_8_16[JD[6]-0x30], 2, StartCol+48);
				zf_disp8x16(Num_8_16[JD[7]-0x30], 2, StartCol+56);
				zf_disp8x16(Num_8_16[JD[8]-0x30], 2, StartCol+64);
				zf_disp8x16(Num_8_16[JD[9]-0x30], 2, StartCol+72);
				Display_Chinese(kong,2,StartCol+80); //1.	  	 
				Display_Chinese(wei2,0,0); //1.
				Display_Chinese(du,0,16); //1.
				Display_Chinese(maohao,0,32); //1.
				zf_disp8x16(Num_8_16[WD[0]-0x30], 0, StartCol+0);
				zf_disp8x16(Num_8_16[WD[1]-0x30], 0, StartCol+8);
				zf_disp8x16(Num_8_16[WD[2]-0x30], 0, StartCol+16);
				zf_disp8x16(Num_8_16[WD[3]-0x30], 0, StartCol+24);
				zf_disp8x16(Num_8_16[10], 0, StartCol+32);
				zf_disp8x16(Num_8_16[WD[5]-0x30], 0, StartCol+40);
				zf_disp8x16(Num_8_16[WD[6]-0x30], 0, StartCol+48);
				zf_disp8x16(Num_8_16[WD[7]-0x30], 0, StartCol+56);
				zf_disp8x16(Num_8_16[WD[8]-0x30], 0, StartCol+64);
				Display_Chinese(kong,0,StartCol+72); //1.
	
		  	   }
			//}
		}
		else if(celiangPage_idx==CELIANG_SN_PAGE)
		{
			get_sn_data();
			LcmClear();
			Display_PD(sn_num_pic);

			zf_disp7x16(Num_7_16[system_data[0]], 1, 33);
			zf_disp7x16(Num_7_16[system_data[1]], 1, 33+7*1);
			zf_disp7x16(Num_7_16[system_data[2]], 1, 33+7*2);
			zf_disp7x16(Num_7_16[system_data[3]], 1, 33+7*3);
			zf_disp7x16(Num_7_16[system_data[4]], 1, 33+7*4);
			zf_disp7x16(Num_7_16[system_data[5]], 1, 33+7*5);
			zf_disp7x16(Num_7_16[system_data[6]], 1, 33+7*6);
			zf_disp7x16(Num_7_16[system_data[7]], 1, 33+7*7);
			zf_disp7x16(Num_7_16[system_data[8]], 1, 33+7*8);
			zf_disp7x16(Num_7_16[system_data[9]], 1, 33+7*9);
			zf_disp7x16(Num_7_16[system_data[10]], 1, 33+7*10);
			zf_disp7x16(Num_7_16[system_data[11]], 1, 33+7*11);

			zf_clear_page_to_end(1,117);
			zf_clear_page_to_end(2,117);
		}
}
void display_danjia_Page()
{
   	LcmClear();
	Update_danjia_page4_5_1_2(danjiasel);

}
void display_danwei_Page()
{
   	LcmClear();
	Update_danwei_page4_5_1_2(danwei_zouchang_sel,danwei_mianji_sel,FLAG4);

}
void Update_jilu_page()
{
  	unsigned char time[5],zouchang[20],mianji[20],TEST_8[20],ItoaBuffer[6];
	unsigned char count,DanweiF;
	unsigned int IndexNewNumBuffer;
	unsigned int danjia;
	unsigned char offset = 0;
	Get_Data(TEST_2,time,zouchang,mianji,TEST_8,&DanweiF, &danjia);
	IndexNewNumBuffer =  TEST_2+1;
	sprintf(ItoaBuffer, "%02d", IndexNewNumBuffer);
	LcmClear();
	if(get_history_data_cnt() == 0)
	{
	  Display_Chinese(wu,6,0); 
	Display_Chinese(ji,6,16); 
	Display_Chinese(lu,6,32); 
	Display_Chinese(kong,6,48); 
	Display_Chinese(kong,6,64);
	Display_Chinese(kong,6,80);
	Display_Chinese(kong,6,96); 
	Display_Chinese(kong,6,112);
	}
	else
	{	
	Display_Chinese(di,6,0);
	zf_disp8x16(Num_8_16[ItoaBuffer[0]-0x30], 6, 16);
	zf_disp8x16(Num_8_16[ItoaBuffer[1]-0x30], 6, 24);
	Display_Chinese(tiao,6,32); 
	Display_Chinese(kong,6,48); 
	Display_Chinese(shi,6,56); 
	Display_Chinese(jian,6,72); 
	zf_disp8x16(Num_8_16[time[0]-0x30], 6, 88);	
	zf_disp8x16(Num_8_16[time[1]-0x30], 6, 96);	
	Display_Chinese(maohao,6,104); 
	zf_disp8x16(Num_8_16[time[2]-0x30], 6, 112); 
	zf_disp8x16(Num_8_16[time[3]-0x30], 6, 120); 

	Display_Chinese(dan,4,0);
	Display_Chinese(jia,4,16);
	if( (unsigned char)(danjia/1000)>0 )
	{
		zf_disp8x16(Num_8_16[(unsigned char)(danjia/1000)], 4, 32+offset);
		offset += 8;
	}
	danjia = danjia%1000;
	if( offset>0 || (unsigned char)(danjia/100)>0 )
	{
		zf_disp8x16(Num_8_16[(unsigned char)(danjia/100)], 4, 32+offset);
		offset += 8;
	}
	danjia = danjia%100;
	if( offset>0 || (unsigned char)(danjia/10)>0 )
	{
		zf_disp8x16(Num_8_16[(unsigned char)(danjia/10)], 4, 32+offset);
		offset += 8;
	}
	danjia = danjia%10;
	zf_disp8x16(Num_8_16[danjia], 4, 32+offset);
	offset += 8;
	Display_Chinese(yuan,4,32+offset);
	offset += 16;
	for( ; 32+offset<128; offset+=8)
	{
		zf_disp8x16(kong, 4, 32+offset);
	}

#if 0
	Display_Chinese(zou,4,0);
	Display_Chinese(chang,4,16);
	
	for(count=0;count<8;count++)
	{
		if(zouchang[count] != 0x2e)
		zf_disp8x16(Num_8_16[zouchang[count]-0x30], 4, 32+count*8);	
		else
		zf_disp8x16(Num_8_16[10], 4, 32+count*8);	
	}
	
	if(0 == (DanweiF&0x01)) 
	{

	Display_Chinese(mi,4,96); 
	Display_Chinese(kong,4,112); 
	 }
	 else
	 {
	  Display_Chinese(qian,4,96); //:
	  Display_Chinese(mi,4,112); //:
	 }
#endif

	Display_Chinese(mian,2,0); //走
	Display_Chinese(ji2,2,16); //长

	for(count=0;count<8;count++)
	{
		if(mianji[count] != 0x2e)
		zf_disp8x16(Num_8_16[mianji[count]-0x30], 2, 32+count*8);	
		else
		zf_disp8x16(Num_8_16[10], 2, 32+count*8);	
	}
	
	if(0 == (DanweiF&0x02))
	{

	Display_Chinese(mu,2,96); //:
	Display_Chinese(kong,2,112); //:
	 }
	 else
	 {
	  Display_Chinese(gong,2,96); //:
	  Display_Chinese(qing,2,112); //:
	 }

	Display_Chinese(jin,0,0); //金
	Display_Chinese(er,0,16); //额
	Display_Chinese(maohao,0,32); //:
	for(count=0;count<8;count++)
	{

		zf_disp8x16(Num_8_16[TEST_8[count]-0x30], 0, 40+count*8);	
	//	else
	//	zf_disp8x16(Num_8_16[10], 0, 40+count*8);	
	}
	

	Display_Chinese(yuan,0,104); //元
	Display_Chinese(kong,0,120); //kong
   }
}
void display_jilu_page()
{
	Update_jilu_page();
    
}

void dispay_sn_edit_page()
{
	unsigned char i=0;

	LcmClear();
	if(sn_focus_idx == 20)
		Revers_Data(hao,4,0);
	else
		Display_Chinese(hao, 4, 0);

	Display_Chinese(maohao,4,16);

	for(i=0; i<SN_NUM_LEN; i++)
	{
		if(i==sn_focus_idx)
		{
			reverse_zf_disp8x16(Num_8_16[system_data[i]], 4, 32+8*i);
		}
		else
		{
			zf_disp8x16(Num_8_16[system_data[i]], 4, 32+8*i);
		}
	}
	zf_clear_page_to_end(4,32+8*SN_NUM_LEN);
	zf_clear_page_to_end(5,32+8*SN_NUM_LEN);

	if(sn_focus_idx == 30)
		Revers_Data(shi3,1,0);
	else
		Display_Chinese(shi3, 1, 0);

	zf_clear_page_to_end(1,16);
	zf_clear_page_to_end(2,16);
}

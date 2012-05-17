#include <REG52.h>
#include<intrins.h>	
#include <stdlib.h>
#include<string.h>

#define JD_BUFF_LEN 11
#define WD_BUFF_LEN 10
#define TIME_BUFF_LEN 7
#define SPEED_BUFF_LEN 6
#define USED_SAT_BUFF_LEN 3
#define TOTAL_SAT_BUFF_LEN 3
#define CMD_BUFF_LEN 5

extern bit TEST4;
extern bit signal;



bit GPS_UPDATA = 0;	
bit GPS_TIME_UPDATE = 0;
bit TOTAL_SAT_UPDATE = 0;

bit GPS_Point_Updata_WD = 0;
bit GPS_Point_Updata_JD = 0;
bit GPS_Point_Updata_SatNum = 0;
bit GPS_Point_Updata_WD_LCD_Fresh = 0;
bit GPS_Point_Updata_JD_LCD_Fresh = 0;
bit GPS_Point_Updata_SatNum_LCD_Fresh = 0;

char code TIME_AREA= 8;


unsigned char JD[JD_BUFF_LEN] = {'0','0','0','0','0','0','0','0','0','0','\0'};
unsigned char JD_LST[JD_BUFF_LEN];
unsigned char JD_a; 
unsigned char WD[WD_BUFF_LEN] = {'0','0','0','0','0','0','0','0','0','\0'};
unsigned char WD_LST[WD_BUFF_LEN];
unsigned char WD_a; 
unsigned char curr_time[TIME_BUFF_LEN];
unsigned char curr_time_LST[TIME_BUFF_LEN];
unsigned char TEST_1[5];
unsigned char speed[SPEED_BUFF_LEN];
unsigned char high[6]; 
unsigned char angle[5]; 
unsigned char use_sat[USED_SAT_BUFF_LEN] = {'0','0','\0'};
unsigned char use_sat_LST[USED_SAT_BUFF_LEN] = {'0','0','\0'};
unsigned char total_sat[TOTAL_SAT_BUFF_LEN]= {'0','0','\0'};
unsigned char total_sat_LST[TOTAL_SAT_BUFF_LEN] = {'0','0','\0'};
unsigned char lock; 
unsigned char lock_LST; 

unsigned char seg_count;
unsigned char dot_count; 
unsigned char byte_count; 
unsigned char cmd_number; 
unsigned char mode; 
unsigned char buf_full; 
unsigned char cmd[CMD_BUFF_LEN];


unsigned int dsp_count; 
unsigned char time_count;
sbit GPS_Power_Ctl = P1^5; 
void gps_init(void);
void gps_play();
void gps_arryInit();

 

void OpenGpsPower()
{
	GPS_Power_Ctl = 1;
}

void CloseGpsPower()
{
	GPS_Power_Ctl = 0;
}

void DataSend(unsigned char SendChar[])
{
	unsigned int strln;
	unsigned int i;


//	TMOD = 0x21; /* TMOD: timer 1, mode 2, 8-bit reload */

//	TMOD &= 0x0F;
//	TMOD |= 0x21;
//	TMOD = 0x21;

	//TH1=0xfd;
//	TH1 = 0xfa; /* TH1: reload value for 4800 baud @ 11.059MHz */	
	//TL1=0xfd;
/*	TR1=1;
	REN=1;
	SM0=0;
	SM1=1;
	EA=1;
 */
//	ES = 0;
//	TI = 0;

	strln=strlen(SendChar);
	for(i=0;i<=strln-1;i++)
	{
		//	ES=0;
			SBUF=SendChar[i];	
			while(!TI);	
			TI=0;
		//	ES=1;
	}

//	ES = 1;
}
 
 
void DataSend_char(unsigned char SendChar)
{

			ES=0;
			SBUF=SendChar;	
			while(!TI);	
			TI=0;
			ES=1;
}



void gps_init() 
{
	SCON = 0x50; 
//	TMOD = 0x21; 
	TMOD &= 0x0F;
	TMOD |= 0x21;
	TH1 = 0xFA;
	TR1 = 1;
//	IE=0x90; 
	IE |= 0x90;
//	ES = 1;
	EA =1 ;
}

void init_recv_buffs()
{
	unsigned char i;

	for(i=0;i<CMD_BUFF_LEN;i++)
	{
		cmd[i] = 0;
	}

	for(i=0;i<USED_SAT_BUFF_LEN-1;i++)
	{
		use_sat[i] = '0';
		use_sat_LST[i] = '0';
	}

	for(i=0;i<TOTAL_SAT_BUFF_LEN-1;i++)
	{
		total_sat[i] = '0';
		total_sat_LST[i] = '0';
	}

	for(i=0;i<JD_BUFF_LEN-1;i++)
	{
		JD[i] = '0';
		JD_LST[i] = '0';
	}

	for(i=0;i<WD_BUFF_LEN-1;i++)
	{
		WD[i] = '0';
		WD_LST[i] = '0';
	}

	for(i=0;i<TIME_BUFF_LEN;i++)
	{
		curr_time[i] = '0';
		curr_time_LST[i] = '0';
	}
}

void uart(void) interrupt 4
{
	unsigned char tmp;
	EA = 0;

	if(RI) 
	{
	tmp=SBUF;

	switch(tmp)
	{
		case '$':
			//init_recv_buffs();
			cmd_number=0; 
			mode=1; 
			byte_count=0; 
			break;
		case ',':
			seg_count++; 
			byte_count=0;
			break;
		case '*':	 
			switch(cmd_number)
				{
					case 1:
						buf_full|=0x01;
						break;
					case 2:
						buf_full|=0x02;
						break;
					case 3:
						buf_full|=0x04;
						break;
				}
			mode=0;
			break;
		default:
			if(mode==1)
			{
				cmd[byte_count]=tmp; 
				if(byte_count>=4) 
				{ 
					if(cmd[0]=='G' && cmd[1]=='P')
					{
						if(cmd[2]=='G')
						{
							if(cmd[3]=='G' && cmd[4]=='A')
							{
								cmd_number=1;
								mode=2;
								seg_count=0;
								byte_count=0;
							}
							else if(cmd[3]=='S' && cmd[4]=='V')
							{
								cmd_number=2;
								mode=2;
								seg_count=0;
								byte_count=0;
							}
							else
							{
								mode = 0;
							}
						}
/*
						else if(cmd[2]=='R')
						{
							if(cmd[3]=='M' && cmd[4]=='C')
							{
								cmd_number=3;
								mode=2;
								seg_count=0;
								byte_count=0;
							}
							else
								mode = 0;
						}
*/
						else
						{
							mode = 0;
						}
					}
					else
					{
						mode = 0;
					}
				}
			}
			else if(mode==2)
			{
				switch (cmd_number)
				{
				case 1: 	// GPGGA
					if(GPS_UPDATA==0)
					{
						switch(seg_count)
						{
						case 1:
							if(byte_count<6)
							{
								curr_time[byte_count]=tmp;
							}
							if(byte_count==5)
							{
								curr_time[byte_count+1]='\0';
								GPS_TIME_UPDATE = 1;
								mode = 0;
							}
							if(curr_time_LST[byte_count] != curr_time[byte_count])
							{
								curr_time_LST[byte_count] = curr_time[byte_count];
							}
							break;

						case 2:
							if(byte_count<9)
							{
								if(((tmp>='0')&&(tmp<='9'))||(tmp == '.'))
									WD[byte_count]=tmp;
							}
							if(byte_count==8)
							{
								WD[byte_count+1]='\0';
								GPS_Point_Updata_WD = 1;
								GPS_Point_Updata_WD_LCD_Fresh = 1;
							}
							if(WD_LST[byte_count] != WD[byte_count])
							{
								WD_LST[byte_count] = WD[byte_count];
							}
							break;

						case 4:
							if(byte_count<10)
							{
								if(((tmp>='0')&&(tmp<='9'))||(tmp == '.'))
									JD[byte_count]=tmp;
							}
							if(byte_count==9)
							{
								JD[byte_count+1]='\0';
								GPS_Point_Updata_JD = 1;
								GPS_Point_Updata_JD_LCD_Fresh = 1;
							}
							if(JD_LST[byte_count] != JD[byte_count])
							{
								JD_LST[byte_count] = JD[byte_count];
							}
							break;

						case 6:
							if(byte_count<1)
							{
								lock=tmp;
							}
							if(lock != lock_LST)
							{
								lock_LST = 	lock;
							}
							if(lock == '1')
								TEST4 =1;
							else
								TEST4 =0;
							break;

						case 7:
							if(byte_count<2)
							{
								if((tmp>='0')&&(tmp<='9'))
									use_sat[byte_count]=tmp;
							}
							if(byte_count == 1)
							{
								use_sat[byte_count+1]='\0';
								GPS_Point_Updata_SatNum = 1;
								GPS_Point_Updata_SatNum_LCD_Fresh = 1;
								GPS_UPDATA = 1;
								mode = 0;
							}
							if(use_sat_LST[byte_count] != use_sat[byte_count])
							{
								use_sat_LST[byte_count] = use_sat[byte_count];
							}
							if((lock == '1')&&((use_sat[0])||(use_sat[1])))
							{
								signal = 1;
							}
							else
							{
								signal = 0;
							}
							break;
						}
					}
					break;

				case 2:
					switch(seg_count)
					{
					case 3:
						if(byte_count<2)
						{
							if((tmp>='0')&&(tmp<='9'))
								total_sat[byte_count]=tmp;
						}
						if(byte_count == 1)
						{
							total_sat[byte_count+1]='\0';
							mode = 0;
							TOTAL_SAT_UPDATE = 1;
						}
						if(total_sat_LST[byte_count] != total_sat[byte_count])
						{
							total_sat_LST[byte_count] = total_sat[byte_count];
						}
						break;
					}
					break;

				case 3: 
					switch(seg_count)
					{
					case 1:
						if(byte_count<6)
						{
							curr_time[byte_count]=tmp;
						}
						if(byte_count==5)
						{
							curr_time[byte_count+1]='\0';
							GPS_UPDATA = 1;
							mode = 0;
						}
						if(curr_time_LST[byte_count] != curr_time[byte_count])
						{
							curr_time_LST[byte_count] = curr_time[byte_count];
						}
						break;

					case 7:
						if(byte_count<5)
						{
							speed[byte_count]=tmp;
						}
						break;
					}
					break;
				}
			}
			byte_count++;
			break;
	}
	}
	RI=0;
 //if(mode == 0)
 //	{
 //   DataSend_char(curr_time[byte_count]);
//	if(mode == 0)

//	DataSend("\r\n");
//	}
	EA = 1;
}


void UTC2BeiJingTime()
{
	unsigned char count,Hour_Beijing;
	Hour_Beijing = 	(curr_time[0]-0x30)*10+(curr_time[1]-0x30)+8;
	if(Hour_Beijing >=24 )
		Hour_Beijing = Hour_Beijing -24;
	TEST_1[1] = 	(Hour_Beijing%10)+0x30;
	Hour_Beijing = Hour_Beijing/10;
	TEST_1[0] =	(Hour_Beijing%10)+0x30;
	for(count = 2;count < 4; count++)
	{
		TEST_1[count] = 	curr_time[count];
	}
	TEST_1[4]='\0';			
}



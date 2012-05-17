#include <intrins.h>
#include <reg52.h> 
#include "own_reg.h"
//#include "lcd1602.h"
#include "e2prom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gps.h"

#define uchar unsigned char
#define uint unsigned int 

#define RdCommand 0x01
#define PrgCommand 0x02
#define EraseCommand 0x03 

#define Error 1

#define Ok 0
#define WaitTime 0x01 


typedef struct Saved_Data
{
	unsigned char Stor_Time[6];
	unsigned char zouchang[6];
	unsigned char mianji[6];
	unsigned char jiner[6];
}Save_Data;

sbit dula=P2^6;  
sbit wela=P2^7;  


sbit lcdrs=P3^5;
sbit lcdrw=P3^1;

uchar table1[16];

uchar code table[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};


unsigned char history_cnt;
unsigned char history_index;
unsigned char system_data[SYSTEM_DATA_SIZE];
unsigned char sn_focus_idx = 0;
unsigned char data_tmp[6];

extern unsigned char TEST2[];

void delay(unsigned int z)
{
	unsigned int x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);	
}
#if 0

void lcdwritecom(unsigned char dat)
{	
	lcden=0;
	P0=dat;
	lcdrs=0;
	lcdrw=0;
	delay(10);
	lcden=1;
	delay(20);
	lcden=0;
}
#endif
#if 0

void lcdwritedat(unsigned char dat)
{	
	lcden=0;
	P0=dat;
	lcdrs=1;
	lcdrw=0;
	delay(10);
	lcden=1;
	delay(20);
	lcden=0;
}
#endif
#if 0

void lcdwritetable(unsigned char add,unsigned char table[],unsigned char len)
{
	unsigned char i;
	lcdwritecom(add);
	for(i=0;i<len;i++)
	{
		lcdwritedat(table[i]);
		table1[i]=table[i];
		delay(2);
	}
}
#endif
#if 0
void lcddisplay(unsigned char add,unsigned char dat)
{
	lcdwritecom(add);
	lcdwritedat(dat);
}
#endif
#if 0

void lcdinit()
{	
	lcdrw=0x00;
	lcdwritecom(0x38);
	delay(10);
	lcdwritecom(0x0c);
	delay(10);
	lcdwritecom(0x06);
	delay(10);
	lcdwritecom(0x80+0x10);
	delay(10);
	lcdwritecom(0x01);	
}

 #endif

#if 0
void eeprom_erase(uchar AddrH)
{
	IAP_ADDRH=AddrH;
	IAP_ADDRL=0x00;
	IAP_CONTR=0x8b;
	IAP_CMD=0x03;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	delay(1);
}


void eeprom_erase_20(uchar AddrL)
{
	IAP_ADDRH=0x20;
	IAP_ADDRL=AddrL;
	IAP_CONTR=0x8b;
	IAP_CMD=0x03;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	delay(1);
}

 
void EEPROM_write(unsigned char EEPROM_dat,unsigned char EEPROM_Addr)
{
//	eeprom_erase(0x20);
	IAP_CONTR=0x8b;
	IAP_ADDRH=0x20;
	IAP_ADDRL=EEPROM_Addr;
	IAP_DATA=EEPROM_dat;
	IAP_CMD=0x02;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	delay(1);
}
#endif

void eeprom_erase(unsigned char AddrH)
{
	unsigned char i;

	IAP_ADDRH=AddrH;
	IAP_ADDRL=0x00;
	IAP_CONTR=0x8b;
	IAP_CMD=0x03;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	for(i =0; i< 3; ++i);
	IAP_ADDRH=0xff;
	IAP_ADDRL=0xff;
	IAP_CONTR=0x00;
	IAP_CMD=0x00;
	IAP_TRIG=0x00;
}


void EEPROM_write(unsigned char EEPROM_dat,unsigned char EEPROM_AddrH,unsigned char EEPROM_AddrL)
{
	unsigned char i;

//	eeprom_erase(0x20);
	IAP_CONTR=0x8b;
	IAP_ADDRH=EEPROM_AddrH;
	IAP_ADDRL=EEPROM_AddrL;
	IAP_DATA=EEPROM_dat;
	IAP_CMD=0x02;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	//delay(1);
	for(i =0; i< 3; ++i);
	IAP_ADDRH=0xff;
	IAP_ADDRL=0xff;
	IAP_CONTR=0x00;
	IAP_CMD=0x00;
	IAP_TRIG=0x00;
}


unsigned char EEPROM_Read(unsigned char EEPROM_AddrH,unsigned char EEPROM_AddrL)
{
	unsigned char rs, i;

	IAP_CONTR=0x8b;
	IAP_ADDRH=EEPROM_AddrH;
	IAP_ADDRL=EEPROM_AddrL;
	IAP_CMD=0x01;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	//delay(1);
	for(i =0; i< 3; ++i);
	IAP_ADDRH=0xff;
	IAP_ADDRL=0xff;
	IAP_CONTR=0x00;
	IAP_CMD=0x00;
	IAP_TRIG=0x00;
	rs = IAP_DATA;

	return rs;
}


#if 0 
void EEPROM_write(unsigned char EEPROM_dat,unsigned char EEPROM_AddrH,unsigned char EEPROM_AddrL)
{

	IAP_CONTR=0x8b;
	IAP_ADDRH=EEPROM_AddrH;
	IAP_ADDRL=EEPROM_AddrL;
	IAP_DATA=EEPROM_dat;
	IAP_CMD=0x02;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	delay(1);
}


unsigned char EEPROM_Read(unsigned char EEPROM_AddrH,unsigned char EEPROM_AddrL)
{
	IAP_CONTR=0x8b;
	IAP_ADDRH=EEPROM_AddrH;
	IAP_ADDRL=EEPROM_AddrL;
	IAP_CMD=0x01;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	delay(1);
	return IAP_DATA;
}
#endif

#if 0 

unsigned char EEPROM_Read(unsigned char EEPROM_Addr)
{
	IAP_CONTR=0x8b;
	IAP_ADDRH=0x20;
	IAP_ADDRL=EEPROM_Addr;
	IAP_CMD=0x01;
	IAP_TRIG=0x5a;
	IAP_TRIG=0xa5;
	delay(1);
	return IAP_DATA;
}
#endif

typedef struct _HistoryData{
	unsigned int time;
	double zouchang;
	double mianji;
	double jiner;
	unsigned int add_data;
	/* Used to store danjia and Danwei selection
	 * bit 15-14: Danwei selection
	 * bit 13-0 : Danjia
	 */
} HistoryData;

void write_one_char(unsigned char data1, unsigned int addr)
{
	unsigned char addH, addL;

	addH = ((addr)>>8)&0x00ff;
	addL = (addr)&0x00ff;
	EEPROM_write(data1, addH, addL);	
}

unsigned char read_one_char(unsigned int addr)
{
	unsigned char addH, addL;

	addH = ((addr)>>8)&0x00ff;
	addL = (addr)&0x00ff;
	return EEPROM_Read(addH, addL);	
}

void write_data_array(unsigned char* p, unsigned int size, unsigned int addr)
{
	unsigned int i;

	for (i = 0; i < size; ++i) {
		write_one_char(p[i], addr+i);	
	}
}

void read_data_array(unsigned char* p, unsigned int size, unsigned int addr)
{
	unsigned int i;

	for (i = 0; i < size; ++i) {
		p[i] = read_one_char(addr+i);	
	}
}

void write_one_history_data(HistoryData* pdata1, unsigned int addr)
{
	write_data_array((unsigned char*)pdata1, sizeof(HistoryData), addr);
}

void read_one_history_data(HistoryData* pdata1, unsigned int addr)
{
	read_data_array((unsigned char*)pdata1, sizeof(HistoryData), addr);
}

unsigned int time_str_to_int(unsigned char time_str[])
{
	unsigned int time_int = 0;

	if ((time_str[0] <'0' || time_str[0] > '2')
		|| (time_str[1] <'0' || time_str[1] > '9')
		|| (time_str[2] <'0' || time_str[2] > '5')
		|| (time_str[3] <'0' || time_str[3] > '9')
	) {
		return 0;
	}

	time_int += (time_str[0] - '0') * 600;
	time_int += (time_str[1] - '0') * 60;
	time_int += (time_str[2] - '0') * 10;
	time_int += (time_str[3] - '0');

	return time_int;
}

char time_int_to_str(unsigned char time_str[], unsigned int time_int)
{
	unsigned char char_int;
	unsigned int time2 = time_int;

	if (time_int >= 24* 60) {
		memset(time_str, '0', 4);
		time_str[4] = '\0';
		return -1;
	}
	else {
		char_int = time2 / 600;
		time_str[0] = char_int + '0';
		time2 -= 600 * char_int;
		char_int = time2 / 60;
		time_str[1] = char_int + '0';
		time2 -= 60 * char_int;
		char_int = time2 / 10;
		time_str[2] = char_int + '0';
		time2 -= 10 * char_int;
		char_int = time2;
		time_str[3] = char_int + '0';
		time_str[4] = '\0';
		return 0;
	}
}

void backup_and_erase_rom(void)
{
	unsigned char read_char;
	unsigned int end, i;

	eeprom_erase(0x02);

	end = HISTORY_DATA_START_ADDR + HISTORY_DATA_MAX_CNT*sizeof(HistoryData);

	for (i = 0; i < end; ++i) {
		read_char = read_one_char(i);
		write_one_char(read_char, i+HISTORY_DATA_BACKUP_START_ADDR);
	}
}

void write_all_data(unsigned char cnt, unsigned char index, HistoryData* his_data, unsigned int data_addr)
{
	unsigned int i;
	HistoryData old_data;
	unsigned int end;
	unsigned char sys_data[SYSTEM_DATA_SIZE];

	backup_and_erase_rom();
	eeprom_erase(0x00);

	read_data_array(sys_data,SYSTEM_DATA_SIZE, SYSTEM_DATA_START_ADDR+HISTORY_DATA_BACKUP_START_ADDR);
	write_data_array(sys_data, SYSTEM_DATA_SIZE, SYSTEM_DATA_START_ADDR);

	write_one_char(cnt, HISTORY_DATA_CNT_ADDR);
	write_one_char(index, HISTORY_DATA_INDEX_ADDR);

	end = HISTORY_DATA_START_ADDR + cnt*sizeof(HistoryData);
	for (i = HISTORY_DATA_START_ADDR; i < end; i+=sizeof(HistoryData)) {
		if (his_data && data_addr == i) {
			write_one_history_data(his_data, i);
		}
		else {
			read_one_history_data(&old_data, i+HISTORY_DATA_BACKUP_START_ADDR);
			write_one_history_data(&old_data, i);
		}
	}
}

void store_system_data(unsigned char *sys_data)
{
	unsigned char read_char;
	unsigned int end, i;

	backup_and_erase_rom();
	eeprom_erase(0x00);

	write_data_array(sys_data, SYSTEM_DATA_SIZE, SYSTEM_DATA_START_ADDR);

	end = HISTORY_DATA_START_ADDR + HISTORY_DATA_MAX_CNT*sizeof(HistoryData);

	for (i = HISTORY_DATA_CNT_ADDR; i < end; ++i) {
		read_char = read_one_char(i+HISTORY_DATA_BACKUP_START_ADDR);
		write_one_char(read_char, i);
	}
}

void store_sn_data()
{
	store_system_data(system_data);
}

void get_sn_data()
{
	unsigned char i;
	bit flag = 0;

	get_system_data(system_data);

	for(i=0; i<SN_NUM_LEN; i++)
	{
		if(system_data[i]>9)
		{
			flag = 1;
			system_data[i] = 0;
		}
	}
	if(flag)
		store_sn_data();

}

void get_system_data(unsigned char *sys_data)
{
	read_data_array(sys_data, SYSTEM_DATA_SIZE, SYSTEM_DATA_START_ADDR);
}

void init_history_data(void)
{
	unsigned char need_write = 0;

	history_cnt = read_one_char(HISTORY_DATA_CNT_ADDR);
	history_index = read_one_char(HISTORY_DATA_INDEX_ADDR);

	if (history_cnt > HISTORY_DATA_MAX_CNT) {
//		DataSend("\r\ninit_history_data-1-\r\n");
		history_cnt = 0;
		need_write = 1;
	}

	if (history_index > history_cnt) {
//		DataSend("\r\ninit_history_data-2-\r\n");
		history_index = history_cnt;				  
		need_write = 1;
	}

	if (need_write) {
		write_all_data(history_cnt, history_index, 0, 0);
	}
}

unsigned char get_history_data_cnt(void)
{
	return history_cnt;
}

//
void Stor_Data(unsigned char Stor_Time[],unsigned char zouchang[],
	unsigned char mianji[],unsigned char jiner[], unsigned char add_data)
{
	unsigned int data_addr;
	HistoryData data1;

	data_addr = HISTORY_DATA_START_ADDR + history_index * sizeof(HistoryData);
	data1.time = time_str_to_int(Stor_Time);
	data1.zouchang = atof(zouchang);
	data1.mianji = atof(mianji);
	data1.jiner = atof(jiner);
	data1.add_data = (add_data&0x3)<<14;
	data1.add_data |= TEST2[0]*1000+TEST2[1]*100+TEST2[2]*10+TEST2[3];

	if (history_cnt >= HISTORY_DATA_MAX_CNT)
		history_cnt = HISTORY_DATA_MAX_CNT;
	else
		++history_cnt;

	++history_index;
	if (history_index>=HISTORY_DATA_MAX_CNT)
		history_index = 0;

	write_all_data(history_cnt, history_index, &data1, data_addr);
}
//

char Get_Data(unsigned char index, unsigned char Stor_Time[],unsigned char zouchang[],
	unsigned char mianji[],unsigned char jiner[], unsigned char* add_data, unsigned int *danjia)
{
	HistoryData data1;
	unsigned data_addr;

	if (index >= history_cnt) {
		return -1;
	}

	if (history_cnt >= HISTORY_DATA_MAX_CNT) {
		index = (history_index + index) % HISTORY_DATA_MAX_CNT;
	}

	data_addr = HISTORY_DATA_START_ADDR + index * sizeof(HistoryData);
	read_one_history_data(&data1, data_addr);

	time_int_to_str(Stor_Time, data1.time);
	sprintf(zouchang, "%08.1f", data1.zouchang);
	sprintf(mianji, "%08.1f", data1.mianji);
	sprintf(jiner, "%08.0f", data1.jiner);
	*add_data = (unsigned char)((data1.add_data>>14)&0x3);
	*danjia = data1.add_data&0x3FFF;

	return 0;
}

void Clear_Data(void)
{
	history_cnt = 0;
	history_index = 0;

	write_all_data(history_cnt, history_index, 0, 0);
}

#if 0

void main()
{
	uchar i;
	lcdinit();
	/*lcdwritetable(0x80,"EEPRAM",6);
	
	for(i=0;i<=16;i++)
	{
		eeprom_erase_20(i);
	}	
	
	for(i=0;i<=6;i++)
	{
	EEPROM_write(table1[i],i);
	//lcdwritedat(table1[i]);
	}	*/

	//while(1)  ;

	lcdwritecom(0x40+0x80);
	i=0;
	while(1) 
	{	
		for(i=0;i<=10;i++)
		{	lcdwritedat(EEPROM_Read(i));
			//lcddisplay(0x40+0x80,EEPROM_Read(i));
		}
		if(i>=10)
		{
		i=0;
		lcdwritecom(0x40+0x80);
		}
		
	}
	/*
	i=EEPROM_Read(0x00);
	
	lcddisplay(0x40+0x80,i+48);
	while(1)
	{
		EEPROM_write(i,0);
		lcddisplay(0x40+0x80,EEPROM_Read(0x00)+48);
		i=i+1;
		if(i==16) i=0;
		delay(1000);
	}	 */
}
#endif

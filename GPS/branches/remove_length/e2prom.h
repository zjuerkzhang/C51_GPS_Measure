#ifndef _e2prom_h_
#define _e2prom_h_

#define SYSTEM_DATA_SIZE 20
#define HISTORY_DATA_MAX_CNT 30

#define HISTORY_DATA_BACKUP_START_ADDR 0x0200
#define SYSTEM_DATA_START_ADDR 0x0000
#define HISTORY_DATA_CNT_ADDR 0x0000+SYSTEM_DATA_SIZE
#define HISTORY_DATA_INDEX_ADDR HISTORY_DATA_CNT_ADDR+1
#define HISTORY_DATA_START_ADDR HISTORY_DATA_CNT_ADDR+2

#define SN_NUM_LEN 11
#define SN_DATA_START_ADDR HISTORY_DATA_START_ADDR

void init_history_data(void);
unsigned char get_history_data_cnt(void);
void Stor_Data(unsigned char Stor_Time[],unsigned char zouchang[],
	unsigned char mianji[],unsigned char jiner[], unsigned char add_data);
char Get_Data(unsigned char index, unsigned char Stor_Time[],unsigned char zouchang[],
	unsigned char mianji[],unsigned char jiner[], unsigned char *add_data, unsigned int *danjia);
void Clear_Data(void);

void eeprom_erase(unsigned char AddrH);

void store_system_data(unsigned char *sys_data);

void get_system_data(unsigned char *sys_data);

void get_sn_data();

void store_sn_data();

#endif

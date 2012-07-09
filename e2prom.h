#ifndef _e2prom_h_
#define _e2prom_h_

#define SYSTEM_DATA_SIZE 20
#define HISTORY_DATA_MAX_CNT 30

#define HISTORY_DATA_BACKUP_START_ADDR 0x0200
#define SYSTEM_DATA_START_ADDR 0x0000
#define HISTORY_DATA_CNT_ADDR 0x0000+SYSTEM_DATA_SIZE
#define HISTORY_DATA_INDEX_ADDR HISTORY_DATA_CNT_ADDR+1
#define HISTORY_DATA_START_ADDR HISTORY_DATA_CNT_ADDR+2

#define SN_NUM_LEN 12
#define SN_DATA_START_ADDR HISTORY_DATA_START_ADDR
#define PRICE_OFFSET       15
#define DANWEI_OFFSET      18
#define RULER_START_POINT_COUNT_OFFSET  19

void init_history_data(void);
unsigned char get_history_data_cnt(void);
void Stor_Data(unsigned char p_time[], unsigned char p_mode, unsigned char p_data1[],
		unsigned char p_data2[], unsigned char p_danwei_sel, unsigned char p_price[]);
char Get_Data(unsigned char index, unsigned char p_time[], unsigned char *p_mode,
		      unsigned char p_data1[], unsigned char p_data2[], unsigned char *p_danwei_sel,
		      unsigned int *p_price);
void Clear_Data(void);

void eeprom_erase(unsigned char AddrH);

void store_system_data(unsigned char *sys_data);

void get_system_data(unsigned char *sys_data);

void get_sn_data();

void store_sn_data();

#endif

#ifndef _gps_h_
#define _gps_h_
void DataSend(unsigned char SendChar[]);
void DataSend_char(unsigned char SendChar);
void UTC2BeiJingTime();
void CloseGpsPower();
void OpenGpsPower();
void gps_init();
#endif

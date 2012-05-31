#ifndef _OWN_REG_h_
#define _OWN_REG_h_
/*  STC Extensions  */
sfr P3M1 = 0xB1;
sfr P3M0 = 0xB2;
sfr P2M1 = 0x95;
sfr P2M0 = 0x96;
sfr P1M1 = 0x91;
sfr P1M0 = 0x92;
sfr P0M1 = 0x93;
sfr P0M0 = 0x94;
sfr IAP_CONTR = 0xC7;

sfr ADC_CONTR = 0xBC;
sfr ADC_RES = 0xBD;
sfr ADC_LOW2 = 0xBE;
sfr P1ASF = 0x9D;
sfr AUXR = 0x8E;

/*
 sfr IAP_DATA=0xe2;  //¼Ä´æÆ÷ÉêÃ÷
 //sfr IAP_ADDRH=0xe3;
 sfr IAP_ADDRH=0xc3;
 //sfr IAP_ADDRL=0xe4;
 sfr IAP_ADDRL=0xc4;
 sfr IAP_CMD=0xe5;
 sfr IAP_TRIG=0xe6;
 //sfr IAP_CONTR=0xe7;			  */
sfr IAP_DATA = 0xc2;
//sfr IAP_ADDRH=0xe3;
sfr IAP_ADDRH = 0xc3;
//sfr IAP_ADDRL=0xe4;
sfr IAP_ADDRL = 0xc4;
sfr IAP_CMD = 0xc5;
sfr IAP_TRIG = 0xc6;

#endif

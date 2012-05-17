#include "REG52.h"
#include<intrins.h>
#include "keyfuc.h"
#include "display.h"
#include "led.h"
#include "e2prom.h"

unsigned char  	KeyPressValue  = 0;	

sbit KEY_LEFT_UP   = P2^1;	
sbit KEY_OK  = P2^2;
sbit KEY_RIGHT_UP     = P2^3;	 
sbit KEY_LEFT_DOWN     = P2^4;	 
sbit KEY_RIGHT_DOWN   = P2^5; 
sbit key_BL   = P2^7;  
sbit Power_key   = P3^2; 
extern unsigned char TEST_9[];
extern unsigned char GetLenthValue[];
extern unsigned char TEST_0[20];
extern unsigned char TEST_2;
extern unsigned char TEST1;
extern unsigned char TEST_6;
extern unsigned char TEST_1[5];
extern  bit FLAG3;

 sbit Led_Flash =  P3^5;	
unsigned char danjiasel = 0;

bit gps_first_point = 1;

bit danwei_zouchang_sel = 0;
bit danwei_mianji_sel = 0;
bit celiangPage_detail = 0; 
unsigned char celiangPage_idx = CELIANG_WORKING_PAGE;
bit FLAG4 = 0;	
bit Cacul_GoOn_F = 0;

unsigned int PowerDownCount = 0;
unsigned char FLAG1 = 0;
unsigned char clear_rec_count = 0;
unsigned char show_sn_count = 0;
bit searching_sat = 1;

extern unsigned char debugF ; 
extern unsigned char TEST3[];
extern unsigned char TEST2[] ;
extern bit GPS_Point_Updata_JD;
extern unsigned char clear_rec_code[7];
extern unsigned char clear_rec_step;
extern unsigned char system_data[SYSTEM_DATA_SIZE];
extern unsigned char sn_focus_idx;

unsigned char keyscan()
{

 /*
	if((!KEY_OK)&&(!KEY_LEFT_UP)&&(!KEY_LEFT_DOWN))	//left
	{
	delay_ms(10);
	if((!KEY_OK)&&(!KEY_LEFT_UP)&&(!KEY_LEFT_DOWN))	//left
		{
			  KeyPressValue = 1;
		}

	}
	if((!KEY_OK)&&(!KEY_LEFT_UP)&&(!KEY_RIGHT_UP))
	{
	delay_ms(10);
	if((!KEY_OK)&&(!KEY_LEFT_UP)&&(!KEY_RIGHT_UP))
	{
	KeyPressValue = 2;
	}
	}
	if((!KEY_OK)&&(!KEY_RIGHT_DOWN)&&(!KEY_RIGHT_UP))
	{
	delay_ms(10);
	if((!KEY_OK)&&(!KEY_RIGHT_DOWN)&&(!KEY_RIGHT_UP))
	{
	KeyPressValue = 3;
	}
	}
	if((!KEY_OK)&&(!KEY_RIGHT_DOWN)&&(!KEY_LEFT_DOWN))
	{
	delay_ms(10);
	if((!KEY_OK)&&(!KEY_RIGHT_DOWN)&&(!KEY_LEFT_DOWN))
	{
	KeyPressValue = 4;
	}
		
	}
	if((!KEY_OK)&&(KEY_RIGHT_DOWN)&&(KEY_LEFT_UP))
	{
	delay_ms(10);
	if((!KEY_OK)&&(KEY_RIGHT_DOWN)&&(KEY_LEFT_UP))
	{
	KeyPressValue = 5;
	}
	}
	while(!KEY_OK)
   	{
		;
	}
	return 	KeyPressValue;
	*/

	if(!KEY_OK)
	{	
		delay_ms(10);
		if(!KEY_OK)
		{
			if(!KEY_LEFT_UP)	
			{
				delay_ms(10);
				if(!KEY_LEFT_UP)
				{
					if(!KEY_LEFT_DOWN)	
					{
						KeyPressValue = 1;	// left
					}
					else if(!KEY_RIGHT_UP)
					{
					 KeyPressValue = 2;		// up
					}	
				}
				
				
			}
			else if(!KEY_RIGHT_DOWN)
			{	delay_ms(10);
				if(!KEY_RIGHT_DOWN)
				{	
					if(!KEY_RIGHT_UP)
					{
						 KeyPressValue = 3;		// right
					}
					else if(!KEY_LEFT_DOWN)	
					{

						KeyPressValue = 4;		// down
					}
				}
			}
			else
			{
				 KeyPressValue = 5;				// ok
			}

		}
		else
		{
			KeyPressValue = 0;		// none key
			
		}
		/*
		while(!KEY_OK)
	   	{
			;
		}
		*/
	   	
	}

	else if(!key_BL)
	{

		KeyPressValue = 6;	// menu
		/*
		delay_ms(20);
		if(!key_BL)
		{
			
			//LCD_BL_Ctl_fuc();
		//	SW_DL_Reset();
		}*/
	}/*
	else if(Power_key)
	{
		PowerDownCount++;//开始计时.
		if(PowerDownCount>5000)
		{
			display_PowerD_LOGO();
			LcmClear();
				
		}
		
		
	}	*/
	return 	KeyPressValue;

}

void KeyOperate()
{
	unsigned char i,DanweiState;
	
	if((TEST1 == 0)||(TEST1 == 1)||(TEST1 == 2)||(TEST1 == 3)||(TEST1 == 4) || (TEST1 == 5))
	{

		switch(TEST1)
		{
			case 0:	
			{
					if((KeyPressValue > 0) &&(KeyPressValue < 5) ) 
					{

						switch(KeyPressValue)
						{
							case 1:	 
							{
								if(TEST_6 == 0)
								{
								TEST_6 = 3;
								}
								else
								{
								TEST_6 = TEST_6 -1;
								}
								
								break;
							}
							case 2:	 
							{
								if(TEST_6 > 1)
								{
								TEST_6 = TEST_6 - 2;
	
								}
								
								break;
							}
							case 3:	  
							{
								if(TEST_6 == 3)
								{
								TEST_6 = 0;
						
								}
								else
								{
								TEST_6 = TEST_6 +1;
								
								}
								
								break;
							}
							case 4:	  
							{
								if(TEST_6 < 2)
								{
								TEST_6 = TEST_6 +2;
								}
								break;
							}
						}
					
						FLAG3 = 1;
					}
						else if(KeyPressValue == 5)	
					{						
						switch(TEST_6)
						{
							case 0:
							celiangPage_idx = CELIANG_WORKING_PAGE;
							gps_first_point = 1;
							GeodeticAreaReset();
							GPS_Point_Updata_JD = 0;

							Cacul_GoOn_F = 0;
							FLAG1 = 0;
							TEST1 = 1;

							break;
							case 1:
							TEST1 = 2;
							break;
							case 2:
							TEST1 = 3;
							break;
							case 3:
							TEST1 = 4; 
							if(get_history_data_cnt())
							{
							TEST_2 = get_history_data_cnt()-1;
							}
							else
							{
							  TEST_2 = 0;
							}
							show_sn_count = 0;
							clear_rec_count = 0;
							break;
							default:
							break;
						}
						FLAG3 = 1;
						
					}
					else
					{
					}
					break; 
			}
			case 1:
			{
				
					switch(KeyPressValue)
						{
							case 1:	
							{
								if(celiangPage_idx > CELIANG_DETAIL_PAGE)
									celiangPage_idx--;
								break;
							}
							case 2:	 
							{
								//Cacul_GoOn_F = ~Cacul_GoOn_F;
								break;
							}
							case 3:	 
							{
								if(celiangPage_idx < CELIANG_SN_PAGE)
									celiangPage_idx++;
								break;
							}
							case 4:	  
							{
								break;
							}
							case 5:
							{
								if(celiangPage_idx == CELIANG_WORKING_PAGE)
								{
									if(2 == FLAG1)
									{
										FLAG1 = 0;
									}
									else if(FLAG1 == 0)
									{
										if(searching_sat)
										{}
										else
										{
											initiate_var();
											FLAG1++;
											gps_first_point = 1;
											GeodeticAreaReset();
											GPS_Point_Updata_JD = 0;
											Cacul_GoOn_F = 0;
										}
									}
									else
									{
										if(gps_first_point != 1)
										{
											DanweiState = 0x00 | danwei_mianji_sel;
											DanweiState<<=1;
											DanweiState =  DanweiState|danwei_zouchang_sel;
											Stor_Data(TEST_1,GetLenthValue,TEST_9,TEST_0,DanweiState);
											FLAG1++;
										}
									}
								}
								break;
							}
						}
						FLAG3 = 1;
				break;
			}



			case 2:	  
			{


				if((KeyPressValue > 0) &&(KeyPressValue < 5) )
					{
			
						switch(KeyPressValue)
						{
							case 1:	
							{
								if((danjiasel == 0)||(danjiasel == 5))
								{
			
								}
								else if(danjiasel == 2 || danjiasel == 7)
								{
									danjiasel -= 2;
								}
								else
								{
								danjiasel--;
								}
								
								break;
							}
							case 2:	 
							{

								if((danjiasel >= 2)&&(danjiasel < 5))
								{
									if(TEST2[danjiasel-1] == 9)
									TEST2[danjiasel-1] = 0;
									else
									TEST2[danjiasel-1]++;
									
								}
								
								else if((danjiasel >= 7)&&(danjiasel < 10))
								{
									if(TEST3[danjiasel-6] == 9)
									TEST3[danjiasel-6] = 0;
									else
									TEST3[danjiasel-6]++;

								}
                                /*
								else if(danjiasel==5)
								{
									danjiasel = 0;	
								}
								*/
								break;
							}
							case 3:	 
							{
								if((danjiasel == 4)||(danjiasel == 9))
								{
					
								}
								else if((danjiasel == 0)||(danjiasel == 5))
								{
									danjiasel += 2;
								}
								else
								{
								
								danjiasel++;
								}
								
								break;
							}
							case 4:	  
							{



								if((danjiasel >= 2)&&(danjiasel < 5))
								{
									if(TEST2[danjiasel-1] == 0)
									TEST2[danjiasel-1] = 9;
									else
									TEST2[danjiasel-1]--;
									
								}
								
								else if((danjiasel >= 7)&&(danjiasel < 10))
								{
									if(TEST3[danjiasel-6] == 0)
									TEST3[danjiasel-6] = 9;
									else
									TEST3[danjiasel-6]--;

								}
								/*
								else if(danjiasel==0)
								{
									danjiasel = 5;	
								}
								*/
								break; 
							}
						}

						FLAG3 = 1;
					}
						else if(KeyPressValue == 5)	
					{						
						if(danjiasel >= 5)
						{
							danjiasel = 5;
							for(i = 0 ;i<4 ;i++)
							{
								TEST2[i]= 0;	
							}
						}
						else
						{
							danjiasel = 0;
							for(i = 0 ;i<4 ;i++)
							{
								TEST3[i]= 0;	
							}
						}
						TEST1 = 0;
						FLAG3 = 1;
						
					}
					else
					{
					}
					
				break;
			}



			case 3:	  
			{
	
			

				if((KeyPressValue > 0) &&(KeyPressValue < 5) ) 
					{
					
						switch(KeyPressValue)
						{
							case 1:	 
							{
								/*
								if(FLAG4 == 0)
								{
								danwei_zouchang_sel= ~danwei_zouchang_sel;	
								}
								else
								*/
								{
								danwei_mianji_sel=~danwei_mianji_sel;
								}
								
								break;
							}
							/*
							case 2:	
							{

								FLAG4 = ~FLAG4;
								break;
							}
							*/
							case 3:	 
							{
								/*
								if(FLAG4 == 0)
								{
								danwei_zouchang_sel= ~danwei_zouchang_sel;	
								}
								else
								*/
								{
								danwei_mianji_sel=~danwei_mianji_sel;
								}
								
								break;
							}
							/*
							case 4:	 
							{

								FLAG4 = ~FLAG4;
								break; 
							}
							*/
						}

						FLAG3 = 1;
					}
						else if(KeyPressValue == 5)
					{						
						TEST1 = 0;
						FLAG3 = 1;
						
					}
					else
					{
					}
					
				break;
			}
			

//
			case 4:	  
			{

				if((KeyPressValue > 0) &&(KeyPressValue < 5) )
					{
	
						switch(KeyPressValue)
						{
							case 1:	 
							{
								if(0 == TEST_2)
								{
									 TEST_2 =	 get_history_data_cnt()-1;
								}
								else
								TEST_2--;
								clear_rec_count = 0;
								show_sn_count = 0;
								break;
							}
							case 2:	
							{
								show_sn_count = 0;
								clear_rec_count++;
								if(clear_rec_count>=10)
								{
									clear_rec_count = 0;
									Clear_Data();
									TEST1 = 0;
									FLAG3 = 1;
								}
								break;
							}
							case 3:	
							{
								if(TEST_2 >= get_history_data_cnt()-1)
								{
									TEST_2 = 0;
								}
								else
								TEST_2++;
								clear_rec_count = 0;
								show_sn_count = 0;
								break;
							}
							case 4:	
							{
								clear_rec_count = 0;
								show_sn_count++;
								if(show_sn_count>=10)
								{
									show_sn_count = 0;
									get_sn_data();
									sn_focus_idx= 20;
									TEST1 = 5;
									FLAG3 = 1;
								}
								break; 
							}
						}

						FLAG3 = 1;
					}
						else if(KeyPressValue == 5)	
					{						
						;
					}
					else
					{
					}
				break;
			}

			case 5:
			{
				switch(KeyPressValue)
				{
					case 1:
					{
						if(sn_focus_idx>0 && sn_focus_idx<=SN_NUM_LEN)
							sn_focus_idx--;
						else if(0 == sn_focus_idx)
							sn_focus_idx = 20;
						else
						{}
						break;
					}

					case 2:
					{
						if(30 == sn_focus_idx)
						{
							sn_focus_idx = 20;
						}
						else if(20 == sn_focus_idx)
						{}
						else
						{
							if(system_data[sn_focus_idx]==9)
								system_data[sn_focus_idx] = 0;
							else
								system_data[sn_focus_idx]++;
						}
						break;
					}

					case 3:
					{
						if(sn_focus_idx<SN_NUM_LEN && sn_focus_idx>=0)
							sn_focus_idx++;
						else if(sn_focus_idx == 20)
							sn_focus_idx = 0;
						else
						{}
						break;
					}

					case 4:
					{
						if( 20 == sn_focus_idx )
						{
							sn_focus_idx = 30;
						}
						else if( 30 == sn_focus_idx )
						{}
						else
						{
							if(system_data[sn_focus_idx]==0)
								system_data[sn_focus_idx] = 9;
							else
								system_data[sn_focus_idx]--;
						}
						break;
					}

					case 5:
					{
						if(30 == sn_focus_idx)
						{
							system_data[0] = 1;
							system_data[1] = 0;
							system_data[2] = 0;
							system_data[3] = 1;
							system_data[4] = 5;
							system_data[5] = 0;
							system_data[6] = 5;
							system_data[7] = 0;
							system_data[8] = 0;
							system_data[9] = 0;
							system_data[10] = 0;
							system_data[11] = 0;
							store_sn_data();
						}
						else
						{
							store_sn_data();
							TEST1 = 0;
						}
						break;
					}

				}

				if(KeyPressValue!=0)
					FLAG3 = 1;
			}

		}
		
	}

	if(!key_BL)
	{
		if(TEST1)
		{

			{
			TEST1 = 0;
			FLAG3 = 1;
			}
		}

	}


	//KeyPressValue = 0;
}


void wait_key_ok_release()
{
	unsigned int i = 2000;
	unsigned char j;

	while((!KEY_OK) && (i>0))
	{
		j=86;
		while(j--);
		i--;
		if((5==KeyPressValue) && (0==i))
		{
			i = 2000;
		}
	}
	KeyPressValue = 0;
}

#include "REG52.h"
#include <intrins.h>
#include "keyfuc.h"
#include "display.h"
#include "led.h"
#include "e2prom.h"
#include "area.h"

sbit KEY_LEFT_UP = P2 ^ 1;
sbit KEY_OK = P2 ^ 2;
sbit KEY_RIGHT_UP = P2 ^ 3;
sbit KEY_LEFT_DOWN = P2 ^ 4;
sbit KEY_RIGHT_DOWN = P2 ^ 5;
sbit key_BL = P2 ^ 7;

unsigned char KeyPressValue = 0;
unsigned char danjia_sel_focus = 0;
bit gps_first_point = 1;
bit danwei_zouchang_sel = 0;
bit danwei_mianji_sel = 0;
unsigned char celiangPage_idx = CELIANG_WORKING_PAGE;
bit FLAG4 = 0;
unsigned char celiang_mode = 0;
unsigned char clear_rec_count = 0;
unsigned char show_sn_count = 0;
bit searching_sat = 1;
unsigned char price_for_edit[] = { 0, 0, 8, 0, 0 };
unsigned char danwei_sel_for_edit = 0;
unsigned char len_moving[2] = {4, 0};

extern unsigned char g_area_value[];
extern unsigned char g_length_value[];
extern unsigned char total_cost_str[20];
extern unsigned char curr_history_idx;
extern unsigned char g_page_id;
extern unsigned char menu_focus_idx;
extern unsigned char g_beijing_time[7];
extern bit g_lcd_refresh;
extern unsigned char price_per_len[];
extern unsigned char price_per_area[];
extern unsigned char system_data[SYSTEM_DATA_SIZE];
extern unsigned char sn_focus_idx;
extern unsigned char danwei_sel;
extern double distance_as_moving;

unsigned char keyscan()
{
	if (!KEY_OK)
	{
		delay_ms(10);
		if (!KEY_OK)
		{
			if (!KEY_LEFT_UP)
			{
				delay_ms(10);
				if (!KEY_LEFT_UP)
				{
					if (!KEY_LEFT_DOWN)
					{
						KeyPressValue = 1; // left
					}
					else if (!KEY_RIGHT_UP)
					{
						KeyPressValue = 2; // up
					}
				}

			}
			else if (!KEY_RIGHT_DOWN)
			{
				delay_ms(10);
				if (!KEY_RIGHT_DOWN)
				{
					if (!KEY_RIGHT_UP)
					{
						KeyPressValue = 3; // right
					}
					else if (!KEY_LEFT_DOWN)
					{
						KeyPressValue = 4; // down
					}
				}
			}
			else
			{
				KeyPressValue = 5; // ok
			}
		}
		else
		{
			KeyPressValue = 0; // none key
		}
	}
	else if (!key_BL)
	{
		KeyPressValue = 6; // menu
	}
	return KeyPressValue;

}

void KeyOperate()
{
	unsigned char i;
	bit flag;

	if ((g_page_id == 0) || (g_page_id == 1) || (g_page_id == 2) || (g_page_id
			== 3) || (g_page_id == 4) || (g_page_id == 5))
	{

		switch (g_page_id)
		{
		case 0:
		{
			if ((KeyPressValue > 0) && (KeyPressValue < 5))
			{

				switch (KeyPressValue)
				{
				case 1:
				{
					if (menu_focus_idx == 0)
					{
						menu_focus_idx = 3;
					}
					else
					{
						menu_focus_idx = menu_focus_idx - 1;
					}

					break;
				}
				case 2:
				{
					if (menu_focus_idx > 1)
					{
						menu_focus_idx = menu_focus_idx - 2;

					}

					break;
				}
				case 3:
				{
					if (menu_focus_idx == 3)
					{
						menu_focus_idx = 0;

					}
					else
					{
						menu_focus_idx = menu_focus_idx + 1;

					}

					break;
				}
				case 4:
				{
					if (menu_focus_idx < 2)
					{
						menu_focus_idx = menu_focus_idx + 2;
					}
					break;
				}
				}

				g_lcd_refresh = 1;
			}
			else if (KeyPressValue == 5)
			{
				switch (menu_focus_idx)
				{
				case 0:
					celiangPage_idx = CELIANG_WORKING_PAGE;
					gps_first_point = 1;
					ResetPointPlusArray();

					celiang_mode = 0;
					g_page_id = 1;

					break;
				case 1:
					for(i=0; i<5; i++)
					{
						price_for_edit[i] = price_per_area[i];
					}
					g_page_id = 2;
					break;
				case 2:
					danwei_sel_for_edit = danwei_sel;
					g_page_id = 3;
					break;
				case 3:
					g_page_id = 4;
					if (get_history_data_cnt())
					{
						curr_history_idx = get_history_data_cnt() - 1;
					}
					else
					{
						curr_history_idx = 0;
					}
					show_sn_count = 0;
					clear_rec_count = 0;
					break;
				default:
					break;
				}
				g_lcd_refresh = 1;

			}
			else
			{
			}
			break;
		}
		case 1:
		{

			switch (KeyPressValue)
			{
			case 1:
			{
				if (celiangPage_idx > CELIANG_DETAIL_PAGE)
					celiangPage_idx--;
				break;
			}
			case 2:
			{
				break;
			}
			case 3:
			{
				if (celiangPage_idx < CELIANG_SN_PAGE)
					celiangPage_idx++;
				break;
			}
			case 4:
			{
				break;
			}
			case 5:
			{
				if (celiangPage_idx == CELIANG_WORKING_PAGE)
				{
					if (2 == celiang_mode)
					{
						celiang_mode = 0;
					}
					else if (celiang_mode == 0)
					{
						if (searching_sat)
						{
						}
						else
						{
							initiate_var(0);
							celiang_mode++;
							gps_first_point = 1;
							ResetPointPlusArray();
						}
					}
					else
					{
						if (gps_first_point != 1)
						{
							Stor_Data(g_beijing_time, g_length_value,
									g_area_value, total_cost_str, danwei_sel);
							celiang_mode++;
						}
					}
				}
				break;
			}
			}
			g_lcd_refresh = 1;
			break;
		}

		case 2:
		{

			if ((KeyPressValue > 0) && (KeyPressValue < 5))
			{

				switch (KeyPressValue)
				{
				case 1:
				{
					if ((danjia_sel_focus == 0) || (danjia_sel_focus == 5))
					{

					}
					else if (danjia_sel_focus == 2 || danjia_sel_focus == 7)
					{
						danjia_sel_focus -= 2;
					}
					else
					{
						danjia_sel_focus--;
					}

					break;
				}
				case 2:
				{

					if ((danjia_sel_focus >= 2) && (danjia_sel_focus < 5))
					{
						if (price_for_edit[danjia_sel_focus - 1] == 9)
							price_for_edit[danjia_sel_focus - 1] = 0;
						else
							price_for_edit[danjia_sel_focus - 1]++;

					}

					else if ((danjia_sel_focus >= 7) && (danjia_sel_focus < 10))
					{
						if (price_per_len[danjia_sel_focus - 6] == 9)
							price_per_len[danjia_sel_focus - 6] = 0;
						else
							price_per_len[danjia_sel_focus - 6]++;

					}
					/*
					 else if(danjia_sel_focus==5)
					 {
					 danjia_sel_focus = 0;
					 }
					 */
					break;
				}
				case 3:
				{
					if ((danjia_sel_focus == 4) || (danjia_sel_focus == 9))
					{

					}
					else if ((danjia_sel_focus == 0) || (danjia_sel_focus == 5))
					{
						danjia_sel_focus += 2;
					}
					else
					{

						danjia_sel_focus++;
					}

					break;
				}
				case 4:
				{

					if ((danjia_sel_focus >= 2) && (danjia_sel_focus < 5))
					{
						if (price_for_edit[danjia_sel_focus - 1] == 0)
							price_for_edit[danjia_sel_focus - 1] = 9;
						else
							price_for_edit[danjia_sel_focus - 1]--;

					}

					else if ((danjia_sel_focus >= 7) && (danjia_sel_focus < 10))
					{
						if (price_per_len[danjia_sel_focus - 6] == 0)
							price_per_len[danjia_sel_focus - 6] = 9;
						else
							price_per_len[danjia_sel_focus - 6]--;

					}
					/*
					 else if(danjia_sel_focus==0)
					 {
					 danjia_sel_focus = 5;
					 }
					 */
					break;
				}
				}

				g_lcd_refresh = 1;
			}
			else if (KeyPressValue == 5)
			{
				if (danjia_sel_focus >= 5)
				{
					danjia_sel_focus = 5;
					for (i = 0; i < 4; i++)
					{
						price_per_area[i] = 0;
					}
				}
				else
				{
					danjia_sel_focus = 0;
					for (i = 0; i < 4; i++)
					{
						price_per_len[i] = 0;
					}
				}

				flag = 0;
				for(i=0; i<4; i++)
				{
					if(price_per_area[i]!=price_for_edit[i])
					{
						price_per_area[i] = price_for_edit[i];
						flag = 1;
					}
				}
				if(flag)
				{
					get_system_data(system_data);
					system_data[PRICE_OFFSET] = price_per_area[1];
					system_data[PRICE_OFFSET + 1] = price_per_area[2];
					system_data[PRICE_OFFSET + 2] = price_per_area[3];
					store_sn_data();
				}
				g_page_id = 0;
				g_lcd_refresh = 1;

			}
			else
			{
			}

			break;
		}

		case 3:
		{

			if ((KeyPressValue > 0) && (KeyPressValue < 5))
			{

				switch (KeyPressValue)
				{
				case 1:
				{
					if( danwei_sel_for_edit > 0 )
						danwei_sel_for_edit--;
					break;
				}

				case 2:
				{
					if( danwei_sel_for_edit > 1 )
						danwei_sel_for_edit-=2;
					break;
				}
				case 3:
				{
					if( danwei_sel_for_edit < 3)
						danwei_sel_for_edit++;
					break;
				}
				case 4:
				{
					if( danwei_sel_for_edit <2 )
						danwei_sel_for_edit+=2;
					break;
				}
				}

				g_lcd_refresh = 1;
			}
			else if (KeyPressValue == 5)
			{
				if(danwei_sel != danwei_sel_for_edit)
				{
					danwei_sel = danwei_sel_for_edit;
					get_system_data(system_data);
					system_data[DANWEI_OFFSET] = danwei_sel;
					store_sn_data();
				}

				g_page_id = 0;
				g_lcd_refresh = 1;

			}
			else
			{
			}

			break;
		}

			//
		case 4:
		{

			if ((KeyPressValue > 0) && (KeyPressValue < 5))
			{

				switch (KeyPressValue)
				{
				case 1:
				{
					if (0 == curr_history_idx)
					{
						curr_history_idx = get_history_data_cnt() - 1;
					}
					else
						curr_history_idx--;
					clear_rec_count = 0;
					show_sn_count = 0;
					break;
				}
				case 2:
				{
					show_sn_count = 0;
					clear_rec_count++;
					if (clear_rec_count >= 10)
					{
						clear_rec_count = 0;
						Clear_Data();
						g_page_id = 0;
						g_lcd_refresh = 1;
					}
					break;
				}
				case 3:
				{
					if (curr_history_idx >= get_history_data_cnt() - 1)
					{
						curr_history_idx = 0;
					}
					else
						curr_history_idx++;
					clear_rec_count = 0;
					show_sn_count = 0;
					break;
				}
				case 4:
				{
					clear_rec_count = 0;
					show_sn_count++;
					if (show_sn_count >= 10)
					{
						show_sn_count = 0;
						get_sn_data();
						sn_focus_idx = 20;
						g_page_id = 5;
						g_lcd_refresh = 1;
					}
					break;
				}
				}

				g_lcd_refresh = 1;
			}
			else if (KeyPressValue == 5)
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
			switch (KeyPressValue)
			{
			case 1:
			{
				if (sn_focus_idx > 0 && sn_focus_idx < SN_NUM_LEN)
					sn_focus_idx--;
				else if (0 == sn_focus_idx)
					sn_focus_idx = 20;
				else if (sn_focus_idx>40 && sn_focus_idx<=42)
					sn_focus_idx--;
				else
				{
				}
				break;
			}

			case 2:
			{
				if (40 == sn_focus_idx)
				{
					sn_focus_idx = 30;
				}
				if (30 == sn_focus_idx)
				{
					sn_focus_idx = 20;
				}
				else if (20 == sn_focus_idx)
				{
				}
				else if (sn_focus_idx==41 || sn_focus_idx==42)
				{
					if (len_moving[sn_focus_idx-41] == 9)
						len_moving[sn_focus_idx-41] = 0;
					else
						len_moving[sn_focus_idx-41]++;
				}
				else
				{
					if (system_data[sn_focus_idx] == 9)
						system_data[sn_focus_idx] = 0;
					else
						system_data[sn_focus_idx]++;
				}
				break;
			}

			case 3:
			{
				if (sn_focus_idx < (SN_NUM_LEN - 1) && sn_focus_idx >= 0)
					sn_focus_idx++;
				else if (sn_focus_idx == 20)
					sn_focus_idx = 0;
				else if (sn_focus_idx == 40 )
					sn_focus_idx = 41;
				else if (sn_focus_idx == 41 )
					sn_focus_idx = 42;
				else
				{
				}
				break;
			}

			case 4:
			{
				if (20 == sn_focus_idx)
				{
					sn_focus_idx = 30;
				}
				else if (30 == sn_focus_idx)
				{
					sn_focus_idx = 40;
				}
				else if (40 == sn_focus_idx)
				{
				}
				else if (sn_focus_idx==41 || sn_focus_idx==42)
				{
					if (len_moving[sn_focus_idx-41] == 0)
						len_moving[sn_focus_idx-41] = 9;
					else
						len_moving[sn_focus_idx-41]--;
				}
				else
				{
					if (system_data[sn_focus_idx] == 0)
						system_data[sn_focus_idx] = 9;
					else
						system_data[sn_focus_idx]--;
				}
				break;
			}

			case 5:
			{
				distance_as_moving = len_moving[0]*1.0 + len_moving[1]/10.0;
				if (30 == sn_focus_idx)
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
					g_page_id = 0;
				}
				break;
			}

			}

			if (KeyPressValue != 0)
				g_lcd_refresh = 1;
		}

		}

	}

	if (!key_BL)
	{
		if (g_page_id)
		{

			{
				g_page_id = 0;
				g_lcd_refresh = 1;
			}
		}

	}

	//KeyPressValue = 0;
}

void wait_key_ok_release()
{
	unsigned int i = 2000;
	unsigned char j;

	while ((!KEY_OK) && (i > 0))
	{
		j = 86;
		while (j--)
			;
		i--;
		if ((5 == KeyPressValue) && (0 == i))
		{
			i = 2000;
		}
	}
	KeyPressValue = 0;
}

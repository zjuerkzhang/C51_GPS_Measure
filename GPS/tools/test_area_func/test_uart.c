#include <stdio.h>

#define JD_BUFF_LEN 11
#define WD_BUFF_LEN 10
#define TIME_BUFF_LEN 7
#define SPEED_BUFF_LEN 6
#define USED_SAT_BUFF_LEN 3
#define TOTAL_SAT_BUFF_LEN 3
#define CMD_BUFF_LEN 5

typedef unsigned char bit;

bit GPS_UPDATA = 0;
bit TOTAL_SAT_UPDATE = 0;
bit GPS_TIME_UPDATE = 0;

bit TEST4 = 0;

bit signal = 0;

unsigned char SBUF;
unsigned char JD[JD_BUFF_LEN] =
{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '\0' };
unsigned char JD_LST[JD_BUFF_LEN];
unsigned char WD[WD_BUFF_LEN] =
{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '\0' };
unsigned char WD_LST[WD_BUFF_LEN];
unsigned char curr_time[TIME_BUFF_LEN];
unsigned char curr_time_LST[TIME_BUFF_LEN];
unsigned char speed[SPEED_BUFF_LEN];
unsigned char use_sat[USED_SAT_BUFF_LEN] =
{ '0', '0', '\0' };
unsigned char use_sat_LST[USED_SAT_BUFF_LEN] =
{ '0', '0', '\0' };
unsigned char total_sat[TOTAL_SAT_BUFF_LEN] =
{ '0', '0', '\0' };
unsigned char total_sat_LST[TOTAL_SAT_BUFF_LEN] =
{ '0', '0', '\0' };
unsigned char lock;
unsigned char lock_LST;

unsigned char seg_count;
unsigned char dot_count;
unsigned char byte_count;
unsigned char cmd_number;
unsigned char mode;
unsigned char buf_full;
unsigned char cmd[CMD_BUFF_LEN];

void uart(void)
{
	unsigned char tmp;

	tmp = SBUF;

	switch (tmp)
	{
	case '$':
		//init_recv_buffs();
		cmd_number = 0;
		mode = 1;
		byte_count = 0;
		break;
	case ',':
		seg_count++;
		byte_count = 0;
		break;
	case '*':
		switch (cmd_number)
		{
		case 1:
			buf_full |= 0x01;
			break;
		case 2:
			buf_full |= 0x02;
			break;
		case 3:
			buf_full |= 0x04;
			break;
		}
		mode = 0;
		break;
	default:
		if (mode == 1)
		{
			cmd[byte_count] = tmp;
			if (byte_count >= 4)
			{
				if (cmd[0] == 'G' && cmd[1] == 'P')
				{
					if (cmd[2] == 'G')
					{
						if (cmd[3] == 'G' && cmd[4] == 'A')
						{
							cmd_number = 1;
							mode = 2;
							seg_count = 0;
							byte_count = 0;
						}
						else if (cmd[3] == 'S' && cmd[4] == 'V')
						{
							cmd_number = 2;
							mode = 2;
							seg_count = 0;
							byte_count = 0;
						}
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
				else
				{
					mode = 0;
				}
			}
		}
		else if (mode == 2)
		{
			switch (cmd_number)
			{
			case 1: // GPGGA
				if (GPS_UPDATA == 0)
				{
					switch (seg_count)
					{
					case 1:
						if (byte_count < 6)
						{
							curr_time[byte_count] = tmp;
						}
						if (byte_count == 5)
						{
							curr_time[byte_count + 1] = '\0';
							GPS_TIME_UPDATE = 1;
						}
						if (curr_time_LST[byte_count] != curr_time[byte_count])
						{
							curr_time_LST[byte_count] = curr_time[byte_count];
						}
						break;

					case 2:
						if (byte_count < 9)
						{
							if (((tmp >= '0') && (tmp <= '9')) || (tmp == '.'))
								WD[byte_count] = tmp;
						}
						if (byte_count == 8)
						{
							WD[byte_count + 1] = '\0';

						}
						if (WD_LST[byte_count] != WD[byte_count])
						{
							WD_LST[byte_count] = WD[byte_count];
						}
						break;

					case 4:
						if (byte_count < 10)
						{
							if (((tmp >= '0') && (tmp <= '9')) || (tmp == '.'))
								JD[byte_count] = tmp;
						}
						if (byte_count == 9)
						{
							JD[byte_count + 1] = '\0';

						}
						if (JD_LST[byte_count] != JD[byte_count])
						{
							JD_LST[byte_count] = JD[byte_count];
						}
						break;

					case 6:
						if (byte_count < 1)
						{
							lock = tmp;
						}
						if (lock != lock_LST)
						{
							lock_LST = lock;
						}
						if (lock == '1')
							TEST4 = 1;
						else
							TEST4 = 0;
						break;

					case 7:
						if (byte_count < 2)
						{
							if ((tmp >= '0') && (tmp <= '9'))
								use_sat[byte_count] = tmp;
							else
								use_sat[byte_count] = '0';
						}
						if (byte_count == 1)
						{
							use_sat[byte_count + 1] = '\0';

							if (lock == '1')
							{
								if ((use_sat[0] - '0') * 10
										+ (use_sat[1] - '0') > 0)
									signal = 1;
								else
									signal = 0;
							}
							else
							{
								use_sat[0] = '0';
								use_sat[1] = '0';
								signal = 0;
							}
							GPS_UPDATA = 1;
							mode = 0;
						}
						if (use_sat_LST[byte_count] != use_sat[byte_count])
						{
							use_sat_LST[byte_count] = use_sat[byte_count];
						}
						break;
					}
				}
				break;

			case 2:
				switch (seg_count)
				{
				case 3:
					if (byte_count < 2)
					{
						if ((tmp >= '0') && (tmp <= '9'))
							total_sat[byte_count] = tmp;
					}
					if (byte_count == 1)
					{
						total_sat[byte_count + 1] = '\0';
						mode = 0;
						TOTAL_SAT_UPDATE = 1;
					}
					if (total_sat_LST[byte_count] != total_sat[byte_count])
					{
						total_sat_LST[byte_count] = total_sat[byte_count];
					}
					break;
				}
				break;

			case 3:
				switch (seg_count)
				{
				case 1:
					if (byte_count < 6)
					{
						curr_time[byte_count] = tmp;
					}
					if (byte_count == 5)
					{
						curr_time[byte_count + 1] = '\0';
						GPS_UPDATA = 1;
						mode = 0;
					}
					if (curr_time_LST[byte_count] != curr_time[byte_count])
					{
						curr_time_LST[byte_count] = curr_time[byte_count];
					}
					break;

				case 7:
					if (byte_count < 5)
					{
						speed[byte_count] = tmp;
					}
					break;
				}
				break;
			}
		}
		byte_count++;
		break;
	}

	//if(mode == 0)
	//	{
	//   DataSend_char(curr_time[byte_count]);
	//	if(mode == 0)

	//	DataSend("\r\n");
	//	}

}
#if 0
void main()
{
	FILE *ptr_file = NULL;
	long file_size;
	char *file_buff = NULL;
	char *ptr = NULL;
	float cal_val;
	int i = 0;

	ptr_file = fopen("gps_sample2.txt", "rt");
	if (NULL == ptr_file)
	{
		printf("/*** Fail to open file! ***/\n");
		return;
	}

	fseek(ptr_file, 0, SEEK_END);
	file_size = ftell(ptr_file);
	rewind(ptr_file);

	file_buff = (char *) malloc(file_size);
	if (file_buff == NULL)
	{
		printf("/*** Fail to allocate memery buffer for BMP file! ***/\n");
		fclose(ptr_file);
		return;
	}

	fread(file_buff, 1, file_size, ptr_file);
	fclose(ptr_file);

	for (ptr = file_buff; (*ptr != '\0' && *ptr != '#'); ptr++)
	{
		SBUF = *ptr;
		uart();
		if (GPS_UPDATA > 0)
		{
			GPS_UPDATA = 0;

			cal_val = (JD[0]-'0')*100 + (JD[1]-'0')*10 + (JD[2]-'0') +
					  ((JD[3]-'0')*10.0 + (JD[4]-'0'))/60.0 +
					  ((JD[6]-'0')*0.1 + (JD[7]-'0')*0.01 + (JD[8]-'0')*0.001+(JD[9]-'0')*0.0001)/60;
            sprintf(JD, "%.6f",cal_val);

            cal_val = (WD[0]-'0')*10 + (WD[1]-'0') +
            		((WD[2]-'0')*10.0 + (WD[3]-'0'))/60.0 +
            		((WD[5]-'0')*0.1 + (WD[6]-'0')*0.01 + (WD[7]-'0')*0.001+(WD[8]-'0')*0.0001)/60;
            sprintf(WD, "%.6f",cal_val);

			printf("&markers=color:blue%%7Clabel:%d%%7C%s,%s", i, WD, JD);
			i++;
			/*
			printf("--------------------------------\n");
			printf("[CMD]   %c%c%c%c%c\n", cmd[0], cmd[1], cmd[2], cmd[3],
					cmd[4]);
			printf("[Time]: %c%c:%c%c:%c%c\n", curr_time[0], curr_time[1],
					curr_time[2], curr_time[3], curr_time[4], curr_time[5]);
			printf("[WD]:   %s\n", WD);
			printf("[JD]:   %s\n", JD);
			printf("[u s]:  %s\n", use_sat);
			printf("[t s]:  %s\n", total_sat);
			*/
		}
	}

	free(file_buff);

}
#endif

#include <stdio.h>
#include <stdlib.h>
#include "area.h"
#include "test_uart.h"

typedef unsigned char bit;

extern unsigned char SBUF;
extern bit GPS_TIME_UPDATE;
extern bit GPS_UPDATA;
extern bit TOTAL_SAT_UPDATE;
extern bit signal;
extern unsigned char curr_time[];
extern unsigned char JD[];
extern unsigned char WD[];
extern unsigned char use_sat[];
extern unsigned char total_sat[];

main()
{
	FILE *ptr_file = NULL;
	long file_size;
	char *file_buff = NULL;
	char *ptr = NULL;
	float cal_val;
	int i = 0;
	CRDGEODETIC point, out_point;
	unsigned char gps_first_point = 1;

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

		if (GPS_TIME_UPDATE == 1)
		{
			printf("--------------------------------------------\n");
			printf("[Time]: %c%c:%c%c:%c%c\n", curr_time[0],curr_time[1],
					                           curr_time[2],curr_time[3],
					                           curr_time[4],curr_time[5]);
			GPS_TIME_UPDATE = 0;
		}

		if (GPS_UPDATA == 1)
		{
			printf("[WD]:   %s\n", WD);
			printf("[JD]:   %s\n", JD);
			printf("[u s]:  %s\n", use_sat);

			if (signal)
			{
				MakeGeodeticByString(&point, JD, WD);
				printf("[WD2]:  %.6f\n", point.latitude);
				printf("[JD2]:  %.6f\n", point.longitude);

				if (gps_first_point) //如果是第一个点的话。
				{
					if (GeodeticPutSamplingPoint(&point, &out_point))
					{
						GeodeticFirstPoint(&out_point);
						GeodeticResetSamplingPoint();
						gps_first_point = 0;
					}
				}
				else
				{
					GeodeticNextPoint(&point);
				}

				printf("[area]: %08.1f\n", GeodeticGetArea());

				printf("[len]:  %08.0f\n", GeodeticGetDistance());

			}
			GPS_UPDATA = 0;
		}

		if (TOTAL_SAT_UPDATE == 1)
		{

		}
	}
	free(file_buff);

}

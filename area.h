#ifndef __AREA_H__
#define __AREA_H__

#define GEODETIC_SAMPLING_POINT	3
#define CARTESIAN_POINT_MAX_DISTANCE 30 
#define CARTESIAN_POINT_MIN_DISTANCE 1 

#define CARTESIAN_AVERAGE_POINT_COUNT 1

#define CARTESIAN_MERGE_POINT_TRIGGER_COUNT 5
#define GEODETIC_MERGE_POINT_RADIUS 2.0
#define GEODETIC_MIN_AREA 100.0
#define GEODETIC_MIN_STEP 40
#define GEODETIC_END_START_MAX_DISTANCE 3

#define POINT_PLUS_ARRAY_LENGTH 5
#define POINT_PLUS_ARRAY_AVR_BUFF_LEN 3

typedef struct Point
{
	double x, y;
} Point;

typedef struct tagCRDCARTESIAN
{
	double x;
	double y;

} CRDCARTESIAN, *PCRDCARTESIAN;

typedef struct tagCRDGEODETIC
{
	double longitude;
	double latitude;

} CRDGEODETIC;

typedef struct tagPointPlus
{
    CRDCARTESIAN point;
    unsigned char used;
} PointPlus;

typedef CRDGEODETIC *PCRDGEODETIC;

void MakeGeodeticByString(PCRDGEODETIC pcg, unsigned char* longitude,
		unsigned char* latitude);

void GeodeticFirstPoint(PCRDGEODETIC pcg);

int GeodeticNextPoint(PCRDGEODETIC pcg2);

double GeodeticGetArea(void);

double GeodeticGetDistance(void);

void GeodeticResetSamplingPoint(void);

unsigned char GeodeticPutSamplingPoint(PCRDGEODETIC pcg,
		PCRDGEODETIC pout_point);

void GeodeticToCartesian(PCRDCARTESIAN pcc, PCRDGEODETIC pcg);

void GeodeticToCartesian2(PCRDCARTESIAN pcc, PCRDGEODETIC pcg,
		PCRDGEODETIC pcg2);

unsigned char PreparePointPlusArray(PCRDGEODETIC p_in_pgp);

void GeodeticAreaReset(void);

void GeodeticNextPointPlus(PCRDGEODETIC p_pgp);

void ResetPointPlusArray();

#endif // __AREA_H__

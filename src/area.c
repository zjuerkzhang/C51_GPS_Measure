#include "area.h"
#include "MATH.H"
#include "reg52.h"

#define GEODETIC_TO_CARTESIAN_USE_MORE_ACCURATE

sbit Led_Flash =  P3^5;	
double a = 6378137;
double e2 = 0.0066943799013;
const double PI = 3.1415926535897932;
double r1, r2;
CRDGEODETIC cg;
CRDCARTESIAN before_point;
double area, distance;

unsigned char sampling_point_cnt = 0;
CRDGEODETIC sampling_points[GEODETIC_SAMPLING_POINT];
unsigned char discard_point_cnt = 0;

CRDCARTESIAN average_points[CARTESIAN_AVERAGE_POINT_COUNT];
unsigned char average_point_cnt = 0;
unsigned char average_point_index = 0;

#if defined(GEODETIC_TO_CARTESIAN_USE_MORE_ACCURATE)
CRDCARTESIAN first_cartesian;
#endif

CRDCARTESIAN merge_points[CARTESIAN_MERGE_POINT_TRIGGER_COUNT+1];
unsigned char is_merging = 0;
unsigned char similar_point_cnt = 0;

unsigned char is_measuring = 0;
unsigned int point_step = 0;

void GeodeticToCartesian (PCRDCARTESIAN pcc, PCRDGEODETIC pcg)
{
#if defined(GEODETIC_TO_CARTESIAN_USE_MORE_ACCURATE)
double L,B;
double l,L0;
double X,Y;
double f,e2,a,b,e12;
double t;
double m;
double q2;
double M,N;
double S;
double A0,B0,C0,D0,E0;
double n;
double a_1_e2;
int zonewide=3;
#define BJ54 54
#define WGS84 84
#define XIAN80 80
int base_type=WGS84;  
const double PI = 3.1415926535897932;

double m0;
double m2;
double m4;
double m6;
double m8;
double a0;
double a2;
double a4;
double a6;
double a8;


L = pcg->longitude;
B = pcg->latitude;
if(WGS84 == base_type)
{
   a = 6378137;
   b = 6356752.3142;
   f = 1/298.257223563;
   e2 = 0.0066943799013;
   e12 = 0.00673949674227;
}
else if(BJ54 == base_type)
{
   a = 6378245;
   b = 6356863.0187730473;
   f = 1/298.3;
   e2 = 0.006693421622966;
   e12 = 0.006738525414683;
}
else if(XIAN80 == base_type)
{
   a = 6378140;
   b = 6356755.2881575287;
   f = 1/298.257;
   e2 = 0.006694384999588;
   e12 = 0.006739501819473;
}

a_1_e2 = a*(1-e2);

   if (zonewide==6) 
    {
         n=(int)(L/6)+1;
         L0=n*zonewide-3;
    }
    else
    {
        n=(int)((L-1.5)/3)+1;
        L0=n*3;
    }


L0=L0*PI/180;
    L=L*PI/180;
    B=B*PI/180;
l = L - L0;
t = tan(B);
q2 = e12*(cos(B)*cos(B));
N = a/sqrt(1-e2*sin(B)*sin(B));
m = l*cos(B);

m0 = a_1_e2;
m2 = 3.0 * e2 * m0 / 2.0;
m4 = 5.0 * e2 * m2 / 4.0;
m6 = 7.0 * e2 * m4 / 6.0;
m8 = 9.0 * e2 * m6 / 8.0;
a0 = m0 + m2 / 2.0 + 3.0 * m4 / 8.0 + 5.0 * m6 / 16.0 + 35.0 * m8 / 128;
a2 = m2 / 2.0 + m4 / 2.0 + 15.0 * m6 / 32.0 + 7.0 * m8 / 16.0;
a4 = m4 / 8.0 + 3.0 * m6 / 16.0 + 7.0 * m8 / 32.0;
a6 = m6 / 32.0 + m8 / 16.0;
a8 = m8 / 128.0;
A0 = a0;
B0 = a2;
C0 = a4;
D0 = a6;
E0 = a8;
S = (A0 * B - B0* sin(2.0*B)/2.0 + C0 * sin(4.0*B)/4.0 - D0 *sin(6.0*B)/6.0 + E0*sin(8.0*B)/8.0);


X = S + N * t *((0.5 + ((5.0 - t * t + 9 * q2 +4 * q2 * q2)/24 + (61.0 - 58.0 * t * t + pow(t,4)) * m * m /720.0) * m * m) * m * m);//pow(t,4)为t的4次方
Y = N * ((1 + ((1 - t * t +q2)/6.0 +(5.0 - 18.0 * t * t + pow(t,4) + 14 * q2 - 58 * q2 * t * t ) * m * m / 120.0) * m * m ) * m);
Y = 1000000 * n + 500000 + Y;
pcc->x = Y;
pcc->y = X;
#endif
}

void GeodeticToCartesian2 (PCRDCARTESIAN pcc, PCRDGEODETIC pcg, PCRDGEODETIC pcg2)
{
	double a = 6378137;
	double b = 6356752.3142;
	double f = 1/298.257223563;
	double e2 = 0.0066943799013;
	double e12 = 0.00673949674227;
	const double PI = 3.1415926535897932;

	double r1, r2, r3;
	double B, L;
	double X, Y;

	B = pcg->latitude * PI/180;
	L = pcg->longitude * PI/180;
	r1 = a/sqrt(1-e2*sin(B)*sin(B));
	r2 = r1 * cos(B);
	r3 = r1 * sin(B);

	X = r2*PI*(pcg2->longitude-pcg->longitude)/180;
	Y = r1*PI*(pcg2->latitude - pcg->latitude)/180;

	pcc->x = X;
	pcc->y = Y;
}

unsigned char GeodeticPutSamplingPoint(PCRDGEODETIC pcg, PCRDGEODETIC pout_point)
{
	char i, j;

	if (sampling_point_cnt >= GEODETIC_SAMPLING_POINT) {	//
		return 0;
	}

	for (i = 0; i < sampling_point_cnt; ++i) {
		if (pcg->longitude <= sampling_points[i].longitude && pcg->latitude <= sampling_points[i].latitude) {
			break;
		}
	}
	 
	for (j = sampling_point_cnt - 1; j >= i; --j) {
		sampling_points[j+1] = sampling_points[j];
		
	}
  // 	Led_Flash = 0;
	sampling_points[i] = *pcg;
	++sampling_point_cnt;

	

	if (sampling_point_cnt >= GEODETIC_SAMPLING_POINT) {
		if (pout_point != 0) {
			*pout_point = sampling_points[(sampling_point_cnt-1)/2];
		}
		return 1;
	}
	else {
		return 0;
	}
}

void GeodeticResetSamplingPoint(void)
{
	sampling_point_cnt = 0;
}
 
void GeodeticResetDiscardPoint(void)
{
	discard_point_cnt = 0;

	average_point_cnt = 0;
	average_point_index = 0;
}

CRDCARTESIAN GeodeticGetAveragePoint(PCRDCARTESIAN pcc)
{
	CRDCARTESIAN ave_point;
	unsigned char i, cnt;

	ave_point.x = ave_point.y = 0;
	average_points[average_point_index++] = *pcc;
	if (average_point_index>=CARTESIAN_AVERAGE_POINT_COUNT) average_point_index = 0;

	if (average_point_cnt < CARTESIAN_AVERAGE_POINT_COUNT) {
		++average_point_cnt;
		cnt = average_point_cnt;
	}
	else {
		cnt = CARTESIAN_AVERAGE_POINT_COUNT;
	}

	for(i = 0; i < cnt; ++i) {
		ave_point.x += average_points[i].x;
		ave_point.y += average_points[i].y;
	}
	ave_point.x /= cnt;
	ave_point.y /= cnt;

	return ave_point;
}

CRDCARTESIAN GeodeticMergeSimilarPoints(PCRDCARTESIAN pcc)
{
	double dx, dy, minx, miny, maxx, maxy, rb_area, rb_distance;
	char i;

	dx = pcc->x - before_point.x;
	dy = pcc->y - before_point.y;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;

	if (is_merging) {
		if (dx <= GEODETIC_MERGE_POINT_RADIUS && dy <= GEODETIC_MERGE_POINT_RADIUS) {
//printf("--dx:%lf\r\n--dy:%lf\r\n", before_point.x, before_point.y);
			return before_point;
		}
		else {
			is_merging = 0;
			merge_points[0] = *pcc;
			similar_point_cnt = 0;
//printf("--dx:%lf\r\n--dy:%lf\r\n", merge_points[0].x, merge_points[0].y);
			return *pcc;
		}
	}

	if (dx > GEODETIC_MERGE_POINT_RADIUS || dy > GEODETIC_MERGE_POINT_RADIUS) {
		merge_points[0] = *pcc;
		similar_point_cnt = 0;

		return *pcc;
	}

	merge_points[++similar_point_cnt] = *pcc;
	if (similar_point_cnt < CARTESIAN_MERGE_POINT_TRIGGER_COUNT) {
		return *pcc;
	}

	minx=maxx=merge_points[1].x;
	miny=maxy=merge_points[1].y;
	dx = dy = 0;
	for(i = 2; i <= CARTESIAN_MERGE_POINT_TRIGGER_COUNT; ++i) {
		if (minx>merge_points[i].x) {
			minx = merge_points[i].x;
		}
		else if (maxx<merge_points[i].x) {
			maxx = merge_points[i].x;
		}
		if (miny>merge_points[i].y) {
			miny = merge_points[i].y;
		}
		else if (maxy<merge_points[i].y) {
			maxy = merge_points[i].y;
		}
		dx = maxx - minx;
		dy = maxy - miny;
		if (dx > GEODETIC_MERGE_POINT_RADIUS * 2 || dy > GEODETIC_MERGE_POINT_RADIUS * 2) {
			break;
		}
	}
	if (dx > GEODETIC_MERGE_POINT_RADIUS * 2 || dy > GEODETIC_MERGE_POINT_RADIUS * 2) {
		for(i = 0; i < CARTESIAN_MERGE_POINT_TRIGGER_COUNT; ++i) {
			merge_points[i] = merge_points[i+1];
		}
		--similar_point_cnt;
//printf("--dx:%lf\r\n--dy:%lf\r\n", merge_points[0].x, merge_points[0].y);
		return *pcc;
	}
	else {
		for(i = 0, rb_area = rb_distance = 0; i < CARTESIAN_MERGE_POINT_TRIGGER_COUNT; ++i) {
			dx = merge_points[i+1].x - merge_points[i].x;
			dy = merge_points[i+1].y - merge_points[i].y;
			rb_area += (merge_points[i].x * merge_points[i+1].y - merge_points[i].y * merge_points[i+1].x) / 2.0;
			rb_distance += sqrt(dx*dx+dy*dy);
		}
		area -= rb_area;
		distance -= rb_distance;
		before_point = merge_points[0];
		merge_points[0].x = (minx + maxx) / 2;
		merge_points[0].y = (miny + maxy) / 2;
		similar_point_cnt = 0;
		is_merging = 1;
//printf("--dx:%lf\r\n--dy:%lf\r\n", merge_points[0].x, merge_points[0].y);
		return merge_points[0];
	}
}

void GeodeticFirstPoint(PCRDGEODETIC pcg)
{
	double B;

	B = pcg->latitude * PI/180;
	r1 = a/sqrt(1-e2*sin(B)*sin(B));
	r2 = r1 * cos(B);

	cg = *pcg;

#if defined(GEODETIC_TO_CARTESIAN_USE_MORE_ACCURATE)
	GeodeticToCartesian(&first_cartesian, pcg);
#endif

	before_point.x = before_point.y = 0;
	area = 0;
	distance = 0;

	merge_points[0].x = merge_points[0].y = 0;
	is_merging = 0;
	similar_point_cnt = 0;

	is_measuring = 1;
	point_step = 0;
}

int GeodeticNextPoint(PCRDGEODETIC pcg2)
{
	CRDCARTESIAN cur_point;
	double dx, dy, max_distance,min_distance;

	if(!is_measuring)
		return 0;
	++point_step;

#if !defined(GEODETIC_TO_CARTESIAN_USE_MORE_ACCURATE)
	cur_point.x = r2*PI*(pcg2->longitude-cg.longitude)/180;
	cur_point.y = r1*PI*(pcg2->latitude - cg.latitude)/180;
#else
	GeodeticToCartesian(&cur_point, pcg2);
	cur_point.x -= first_cartesian.x;
	cur_point.y -= first_cartesian.y;
#endif

	dx = cur_point.x - before_point.x;
	dy = cur_point.y - before_point.y;
	max_distance = CARTESIAN_POINT_MAX_DISTANCE*(1+discard_point_cnt);
	min_distance = CARTESIAN_POINT_MIN_DISTANCE;//下一个如果在原点1m左右走动的话舍掉。
	if (dx < 0)	dx = -dx;
	if (dy < 0) dy = -dy;
	if (dx > max_distance || dy > max_distance) {
		++discard_point_cnt;
		return -1;
	}
#if 0
	else if (dx < min_distance || dy < min_distance) {
	//	++discard_point_cnt;
		return -2;
	}
#endif
	else {
		discard_point_cnt = 0;
	}

	cur_point = GeodeticGetAveragePoint(&cur_point);

	cur_point = GeodeticMergeSimilarPoints(&cur_point);

	dx = cur_point.x - before_point.x;
	dy = cur_point.y - before_point.y;
	area += (before_point.x * cur_point.y - before_point.y * cur_point.x) / 2.0;
	distance += sqrt(dx*dx+dy*dy);

	if ((area > GEODETIC_MIN_AREA || area < -GEODETIC_MIN_AREA) && point_step > GEODETIC_MIN_STEP) {
		if (cur_point.x < GEODETIC_END_START_MAX_DISTANCE && cur_point.x > -GEODETIC_END_START_MAX_DISTANCE
			&& cur_point.y < GEODETIC_END_START_MAX_DISTANCE && cur_point.y > -GEODETIC_END_START_MAX_DISTANCE) {
				is_measuring = 0;
//printf("--end measuring, the area is %lf, the distance is %lf\r\n", area, distance);
			}
	}

	before_point = cur_point;
	return 0;

}

double GeodeticGetArea(void)
{
	if (area >= 0)	
		return area;
	else
		return -area;
}

double GeodeticGetDistance(void)
{
	return distance;
}

void MakeGeodeticByString(PCRDGEODETIC pcg, unsigned char* longitude, unsigned char* latitude)
{
	pcg->longitude = (longitude[0]-'0')*100+(longitude[1]-'0')*10+(longitude[2]-'0')+((longitude[3]-'0')*10+(longitude[4]-'0')+(longitude[6]-'0')*0.1+(longitude[7]-'0')*0.01+(longitude[8]-'0')*0.001+(longitude[9]-'0')*0.0001)/60;
	pcg->latitude = (latitude[0]-'0')*10+(latitude[1]-'0')+((latitude[2]-'0')*10+(latitude[3]-'0')+(latitude[5]-'0')*0.1+(latitude[6]-'0')*0.01+(latitude[7]-'0')*0.001+(latitude[8]-'0')*0.0001)/60;
}

void GeodeticAreaReset(void)
{
	GeodeticResetSamplingPoint();
	GeodeticResetDiscardPoint();
}

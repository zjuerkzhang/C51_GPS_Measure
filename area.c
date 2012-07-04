#include "area.h"
#include "MATH.H"
#include "reg52.h"

#define GEODETIC_TO_CARTESIAN_USE_MORE_ACCURATE

double r_a = 6378137;
double r_b = 6356752.314;
double e2 = 0.0066943799013;
const double PI = 3.1415926535897932;

CRDCARTESIAN before_point;
double area, distance;

#if defined(GEODETIC_TO_CARTESIAN_USE_MORE_ACCURATE)
CRDCARTESIAN first_cartesian;
#endif

PointPlus point_plus_array[POINT_PLUS_ARRAY_LENGTH];
unsigned char ppa_new = 0;
unsigned char ppa_old = 0;
double distance_as_moving = 4.0;

unsigned char spa_idx = 0;
unsigned char spa_len = 3;

double GetDistanceBetweenPoints(CRDCARTESIAN p_p1, CRDCARTESIAN p_p2)
{
    return sqrt(pow(p_p1.x - p_p2.x, 2) + pow(p_p1.y - p_p2.y, 2));
}


void GeodeticToCartesian(PCRDCARTESIAN pcc, PCRDGEODETIC pcg)
{

    double S, N, t, t2, m, m2, ng2;
    double sinB, cosB;
    double e2, e12;
    double A0, B0, C0, D0, E0;
    double L_rad, B_rad, L0;
    double n0;
    double x, y;
    
    e2 = (r_a*r_a - r_b*r_b)/(r_a*r_a);
    e12 = (r_a*r_a - r_b*r_b)/(r_b*r_b);
    n0 = (r_a - r_b)/(r_a + r_b);
    
    L_rad = pcg->longitude * PI /180.0;
	B_rad = pcg->latitude * PI /180.0;
	if(L_rad < 0.0 )
	{
	    L0 = (unsigned char)((180.0 + pcg->longitude) / 6) + 1;
	}
	else
	{
	    L0 = (unsigned char)(pcg->longitude / 6) + 31;
	}
	L0 = L0*6 - 3 - 180;
    
    A0 = (r_a + r_b)*(1 + pow(n0, 2)/4.0 + pow(n0, 4)/64.0 )/2;
    B0 = A0*( -3/2.0*n0 + 9/16.0*pow(n0, 3) - 3/32.0*pow(n0, 5) );
    C0 = A0*(15/16.0*pow(n0,2) - 15/32.0*pow(n0,4));
    D0 = A0*(-35/48.0*pow(n0,3) + 105/256.0*pow(n0,5));
    E0 = A0*315/512.0*pow(n0,4);
    /*
    printf("A0: %f\n", A0);
    printf("B0: %f\n", B0);
    printf("C0: %f\n", C0);
    printf("D0: %f\n", D0);
    printf("E0: %f\n", E0);
    printf("L0: %f\n", L0);
    */
    S = A0*B_rad + B0*sin(2 * B_rad) + C0 * sin(4 * B_rad) + 
        D0 * sin(6 * B_rad) + E0 * sin(8*B_rad);
    sinB = sin(B_rad);
    cosB = cos(B_rad);
    t = tan(B_rad);
    t2 = t * t;
    N = r_a / sqrt(1 - e2 * sinB * sinB);
    m = cosB * (pcg->longitude - L0)*PI/180.0;
    m2 = m * m;
    ng2 = cosB * cosB * e2 / (1 - e2);
    x = S + N * t * ((0.5 + ((5 - t2 + 9 * ng2 + 4 * ng2 * ng2) / 24.0 + (61 -
                            58 * t2 + t2 * t2) * m2 / 720.0) * m2) * m2);
    y = N * m * ( 1 + m2 * ( (1 - t2 + ng2) / 6.0 + m2 * ( 5 - 18 * t2 + t2 * t2 + 14 * ng2 - 58 * ng2 * t2 ) / 120.0));
    y += 500000;
    /*
    printf("--------------------\n");
    printf("x:%8f y:%8f\n", x, y);
    */
    pcc->x = y;
    pcc->y = x;

}

void GeodeticFirstPoint()
{
	unsigned char i;

    first_cartesian.x = 0;
    first_cartesian.y = 0;
    for(i=0; i<POINT_PLUS_ARRAY_LENGTH; i++)
    {
        first_cartesian.x += point_plus_array[i].point.x;
        first_cartesian.y += point_plus_array[i].point.y;
    }
    first_cartesian.x = first_cartesian.x/POINT_PLUS_ARRAY_LENGTH;
    first_cartesian.y = first_cartesian.y/POINT_PLUS_ARRAY_LENGTH;
    
    for(i=0; i<POINT_PLUS_ARRAY_LENGTH; i++)
    {
        point_plus_array[i].point.x -= first_cartesian.x;
        point_plus_array[i].point.y -= first_cartesian.y;
    }
    
	before_point.x = before_point.y = 0;
	area = 0;
	distance = 0;
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

void MakeGeodeticByString(PCRDGEODETIC pcg, unsigned char* longitude,
		unsigned char* latitude)
{
	pcg->longitude = (longitude[0] - '0') * 100 + (longitude[1] - '0') * 10
			+ (longitude[2] - '0') + ((longitude[3] - '0') * 10 + (longitude[4]
			- '0') + (longitude[6] - '0') * 0.1 + (longitude[7] - '0') * 0.01
			+ (longitude[8] - '0') * 0.001 + (longitude[9] - '0') * 0.0001)
			/ 60;
	pcg->latitude = (latitude[0] - '0') * 10 + (latitude[1] - '0')
			+ ((latitude[2] - '0') * 10 + (latitude[3] - '0') + (latitude[5]
					- '0') * 0.1 + (latitude[6] - '0') * 0.01 + (latitude[7]
					- '0') * 0.001 + (latitude[8] - '0') * 0.0001) / 60;
}

unsigned char PreparePointPlusArray(PCRDGEODETIC p_in_pgp)
{
    CRDCARTESIAN pcp;
    
    GeodeticToCartesian(&pcp, p_in_pgp);
    if( ppa_new>0 && 
        GetDistanceBetweenPoints(pcp, point_plus_array[ppa_new-1].point)>= CARTESIAN_POINT_MAX_DISTANCE )
    {
        return 0;
    }
    point_plus_array[ppa_new].point = pcp;
    point_plus_array[ppa_new].used = 0;
    ppa_new++;
    
    if( POINT_PLUS_ARRAY_LENGTH == ppa_new )
    {
        ppa_new--;        
        ppa_old = POINT_PLUS_ARRAY_AVR_BUFF_LEN-1;
        return 1;
    }
    
    return 0;
}

unsigned char GetNextPointIndex(unsigned char p_idx)
{
    p_idx++;
    if(POINT_PLUS_ARRAY_LENGTH==p_idx)
        p_idx = 0;
        
    return p_idx;
}

unsigned char GetPreviousPointIndex(unsigned char p_idx)
{   
    if(0==p_idx)
        p_idx = POINT_PLUS_ARRAY_LENGTH-1;
    else
        p_idx--;
         
    return p_idx;
}

void GeodeticNextPointPlus(PCRDGEODETIC p_pgp)
{
    CRDCARTESIAN cur_point;
    CRDCARTESIAN base_point;
	double dx, dy;
	unsigned char i,j;
	
	GeodeticToCartesian(&cur_point, p_pgp);
	cur_point.x -= first_cartesian.x;
	cur_point.y -= first_cartesian.y;
	
	dx = cur_point.x - before_point.x;
	dy = cur_point.y - before_point.y;
	
	if( GetDistanceBetweenPoints(cur_point, point_plus_array[ppa_new].point)>CARTESIAN_POINT_MAX_DISTANCE )
	{
	    return;
	}
	
	base_point.x = base_point.y = 0;
	j = ppa_old;
	for(i=0; i<POINT_PLUS_ARRAY_AVR_BUFF_LEN; i++)
	{
	    if( point_plus_array[j].used )
	    {
	        base_point = point_plus_array[j].point;
	        break;
	    }
	    base_point.x += point_plus_array[j].point.x;
	    base_point.y += point_plus_array[j].point.y;
	    j = GetPreviousPointIndex(j);
	}
	if( i>= POINT_PLUS_ARRAY_AVR_BUFF_LEN )
	{
	    base_point.x = base_point.x/POINT_PLUS_ARRAY_AVR_BUFF_LEN;
	    base_point.y = base_point.y/POINT_PLUS_ARRAY_AVR_BUFF_LEN;
	}
	
	ppa_old = GetNextPointIndex(ppa_old);
	ppa_new = GetNextPointIndex(ppa_new);
	point_plus_array[ppa_new].point = cur_point;
	point_plus_array[ppa_new].used = 0;
	
	if(GetDistanceBetweenPoints(base_point, cur_point)>distance_as_moving)
	{
	    area += (before_point.x * cur_point.y - before_point.y * cur_point.x) / 2.0;
	    distance += sqrt(dx * dx + dy * dy);
	    
	    point_plus_array[ppa_new].used = 1;
	    before_point = cur_point;
	}
}

void ResetPointPlusArray()
{
	ppa_new = 0;
	ppa_old = 0;
}

#include "REG52.h"
#include<intrins.h>
sbit LCD_BL = P3^4; 

bit lcd_bl_ctl = 1;

void LCD_BL_Ctl_fuc()
{
	 LCD_BL = 1;
}
#include "REG52.h"
#include<intrins.h>
sbit LCD_BL = P3^4; 

bit lcd_bl_ctl = 1;
 sbit Led_Flash =  P3^5;

void LCD_BL_Ctl_fuc()
{
	 LCD_BL = 1;
//	 Led_Flash = ~Led_Flash;
}
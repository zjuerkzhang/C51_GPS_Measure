#ifndef _display_h_
#define _display_h_

void delay_ms(unsigned int ms);
void LCD_Reset();
void Write_Data(unsigned char dat);
void Write_Instruction(unsigned char cmd);
void Set_Page_Address(unsigned char page);
void Set_Column_Address(unsigned char Column);
void LcmClear(void);
void Initial_lcd(void);

void Display_Chinese(unsigned char ft[], unsigned char page,
		unsigned char column, unsigned int inverse);
void zf_disp8x16(unsigned char ft[], unsigned char page, unsigned char column);
void zf_disp7x16(unsigned char ft[], unsigned char page, unsigned char column);
void zf_disp8x8(unsigned char ft[], unsigned char page, unsigned char column);
void zf_disp16x8(unsigned char ft[], unsigned char page, unsigned char column);
void display_chinesestring();
void display_bat();
void CGRAM();
void DisplayCGRAM(unsigned char x, unsigned char y);

void display_Idle();
void Display(unsigned char *p);
void Initial_lcd_bank(void);
void Update_Idle_Page7(unsigned int Timer_L, unsigned int BatQuan_L,
		unsigned int StarNum_L, bit signal_L);
void display_LOGO();
void display_danwei_Page();
void display_danjia_Page();
void display_CeLiang_Page(bit timer_fresh);
void display_PowerD_LOGO();
void display_jilu_page();
void Display_PD(unsigned char *p);
void initiate_var(bit p_start);
#endif

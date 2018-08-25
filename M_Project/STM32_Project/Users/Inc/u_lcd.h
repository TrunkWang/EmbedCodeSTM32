#ifndef _U_LCD_H_
#define _U_LCD_H_




void u_lcd_driver_writecmd(unsigned short cmd);
void u_lcd_driver_writedata( unsigned short data);

void u_lcd_12864_init();
void u_lcd_12864_ClearScreen(void);
unsigned char u_lcd_12864_WriteString(unsigned char x, unsigned char y, unsigned char *cn,unsigned char len);
unsigned char u_lcd_12864_PrintString(unsigned char *str,unsigned char len);


#endif
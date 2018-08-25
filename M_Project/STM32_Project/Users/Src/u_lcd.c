#include "string.h"

#include "u_lcd.h"
#include "u_codec.h"

#include"stm32f10x.h"






#define DATA_PORT  (GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15)//PB
#define LCD12864_CS   (GPIO_Pin_8) //PA8
#define LCD12864_RSET (GPIO_Pin_11) //PA11
#define LCD12864_RS (GPIO_Pin_1) //PB1
#define LCD12864_RW (GPIO_Pin_0) //PB0
#define LCD12864_RD (GPIO_Pin_2) //PB2




unsigned char g_lcd_text[4][16];
int g_line_pos=0;
int g_max_line= 0;




void u_lcd_driver_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = LCD12864_CS | LCD12864_RSET;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	GPIO_InitStructure.GPIO_Pin = DATA_PORT | LCD12864_RS | LCD12864_RW | LCD12864_RD ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void u_lcd_driver_writecmd( unsigned short cmd)
{		
	unsigned char i;
	uint16_t setValue;
	
	GPIO_WriteBit(GPIOA, LCD12864_CS, Bit_RESET);
	
	GPIO_WriteBit(GPIOB, LCD12864_RD, Bit_SET);
	GPIO_WriteBit(GPIOB, LCD12864_RS, Bit_RESET);
	GPIO_WriteBit(GPIOB, LCD12864_RW, Bit_RESET);
	
	setValue = GPIO_ReadOutputData(GPIOB);
	setValue &= 0x00ff;//我们使用的是16位中的高8位		
	setValue |= (uint16_t)cmd << 8;

	GPIO_Write(GPIOB,setValue);


	GPIO_WriteBit(GPIOB, LCD12864_RW, Bit_SET);
}




void u_lcd_driver_writedata( unsigned short data)
{
	unsigned char i;
	uint16_t setValue;
	
	GPIO_WriteBit(GPIOA, LCD12864_CS, Bit_RESET);
	GPIO_WriteBit(GPIOB, LCD12864_RD, Bit_SET);
	GPIO_WriteBit(GPIOB, LCD12864_RS, Bit_SET);
	GPIO_WriteBit(GPIOB, LCD12864_RW, Bit_RESET);
	
	setValue = GPIO_ReadOutputData(GPIOB);
	setValue &= 0x00ff;//我们使用的是16位中的高8位		
	setValue |= (uint16_t)data << 8;

	GPIO_Write(GPIOB,setValue);


	GPIO_WriteBit(GPIOB, LCD12864_RW, Bit_SET);

}



void u_lcd_12864_init()
{
	unsigned char i=0;
	u_lcd_driver_init();
	

	GPIO_WriteBit(GPIOA, LCD12864_RSET, Bit_RESET);
	//LCD12864_RSET = 0;

	
	GPIO_WriteBit(GPIOA, LCD12864_CS, Bit_RESET);
	//LCD12864_CS = 0;
	GPIO_WriteBit(GPIOA, LCD12864_RSET, Bit_SET);
	//LCD12864_RSET = 1;
	
	//----------------Star Initial Sequence-------//
	//------程序初始化设置，具体命令可以看文件夹下---//

	//--软件初始化--//
	u_lcd_driver_writecmd(0xE2);  //reset

	//--表格第8个命令，0xA0段（左右）方向选择正常方向（0xA1为反方向）--//
	u_lcd_driver_writecmd(0xA1);  //ADC select segment direction 
	
	//--表格第15个命令，0xC8普通(上下)方向选择选择反向，0xC0为正常方向--// 
	u_lcd_driver_writecmd(0xC8);  //Common direction 
	                  
	//--表格第9个命令，0xA6为设置字体为黑色，背景为白色---//
	//--0xA7为设置字体为白色，背景为黑色---//
	u_lcd_driver_writecmd(0xA6);  //reverse display

	//--表格第10个命令，0xA4像素正常显示，0xA5像素全开--//
	u_lcd_driver_writecmd(0xA4);  //normal display
	
	//--表格第11个命令，0xA3偏压为1/7,0xA2偏压为1/9--//
	u_lcd_driver_writecmd(0xA2);  //bias set 1/9
	
	//--表格第19个命令，这个是个双字节的命令，0xF800选择增压为4X;--//
	//--0xF801,选择增压为5X，其实效果差不多--//	
	u_lcd_driver_writecmd(0xF8);  //Boost ratio set
	u_lcd_driver_writecmd(0x01);  //x4
	
	//--表格第18个命令，这个是个双字节命令，高字节为0X81，低字节可以--//
	//--选择从0x00到0X3F。用来设置背景光对比度。---/
	u_lcd_driver_writecmd(0x81);  //V0 a set
	u_lcd_driver_writecmd(0x23);

	//--表格第17个命令，选择调节电阻率--//
	u_lcd_driver_writecmd(0x25);  //Ra/Rb set
	
	//--表格第16个命令，电源设置。--//
	u_lcd_driver_writecmd(0x2F);

	//--表格第2个命令，设置显示开始位置--//
	u_lcd_driver_writecmd(0x40);  //start line

	//--表格第1个命令，开启显示--//
	u_lcd_driver_writecmd(0xAF);  // display on
	
	
	
	
}
void u_lcd_12864_ClearScreen(void)
{

	unsigned char i, j;

	for(i=0; i<8; i++)
	{
		//--表格第3个命令，设置Y的坐标--//
		//--Y轴有64个，一个坐标8位，也就是有8个坐标--//
		//所以一般我们使用的也就是从0xB0到0x07,就够了--//	
		u_lcd_driver_writecmd(0xB0+i); 

		//--表格第4个命令，设置X坐标--//
		//--当你的段初始化为0xA1时，X坐标从0x10,0x04到0x18,0x04,一共128位--//
		//--当你的段初始化为0xA0时，X坐标从0x10,0x00到0x18,0x00,一共128位--//
		//--在写入数据之后X坐标的坐标是会自动加1的，我们初始化使用0xA0所以--//
		//--我们的X坐标从0x10,0x00开始---//
		u_lcd_driver_writecmd(0x10); 
		u_lcd_driver_writecmd(0x04);							   
		
		//--X轴有128位，就一共刷128次，X坐标会自动加1，所以我们不用再设置坐标--//
		for(j=0; j<128; j++)
		{
			u_lcd_driver_writedata(0x00);  //如果设置背景为白色时，清屏选择0XFF
		}
	}
}


unsigned char u_lcd_12864_WriteString(unsigned char x, unsigned char y, unsigned char *cn,unsigned char len)
{
	char g_led_print[16];
	unsigned char i=0;
	unsigned char j, x1, x2, wordNum;

	//--Y的坐标只能从0到7，大于则直接返回--//
	if(y > 7)
	{
		return 0;
	}

	//--X的坐标只能从0到128，大于则直接返回--//
	if(x > 128)
	{
		return 0;
	}
	
	y += 0xB0;	   //求取Y坐标的值
	//--设置Y坐标--//
	u_lcd_driver_writecmd(y);
	
	for(i=0;i<16;i++)
	{
		if(i<len)
		{
			g_led_print[i]=cn[i];
		}
		else
		{
			g_led_print[i]=0x00;
		}
	}
	
	for(i=0;i<16;i++)
	{
		//--设置Y坐标--//
		u_lcd_driver_writecmd(y);
		
		x1 = (x >> 4) & 0x0F;   //由于X坐标要两句命令，分高低4位，所以这里先取出高4位
		x2 = x & 0x0F;          //去低四位
		//--设置X坐标--//
		u_lcd_driver_writecmd(0x10 + x1);   //高4位
		u_lcd_driver_writecmd(0x04 + x2);	//低4位
		

		for (j=0; j<16; j++) //写一个字
		{		
				if (j == 8)	 //由于16X16用到两个Y坐标，当大于等于16时，切换坐标
				{
						//--设置Y坐标--//
			   		u_lcd_driver_writecmd(y + 1);
			
						//--设置X坐标--//
						u_lcd_driver_writecmd(0x10 + x1);   //高4位
						u_lcd_driver_writecmd(0x04 + x2);	//低4位
				}
				if((g_led_print[16-1-i] >= 'a') && (g_led_print[16-1-i] <= 'z') )
				{
					u_lcd_driver_writedata(CN16CHAR[g_led_print[16-1-i]-'a'].Msk[j]);
				}
				else
				{
					u_lcd_driver_writedata(CN16CHAR[26].Msk[j]);
				}
				
		}
		x += 8;
	}
	
	return 1;
}



unsigned char u_lcd_12864_PrintString(unsigned char *str,unsigned char len)
{
	int i=4;
	int j=g_line_pos;
	int x=0,y=0;
	int inlen=0;
	if(len == 0)
		return 0;
	
	for(i=0;i<16;i++)
	{
		if(i<len)
		{
			g_lcd_text[g_line_pos][i]=str[i];
		}
		else
		{
			g_lcd_text[g_line_pos][i]=0x00;
		}
	}
	g_max_line++;
	if(g_max_line > 4)
		g_max_line=5;
	
	g_line_pos++;
	g_line_pos = g_line_pos > 3 ? 0:g_line_pos;

	
	if(g_max_line>4)
	{
		j=g_line_pos;
		if(j > 3)
		{
			j = 0;
		}
		u_lcd_12864_WriteString(x,y,g_lcd_text[j],16);j++; j = j > 3 ? 0 : j;
		u_lcd_12864_WriteString(x,y+2,g_lcd_text[j],16);;j++; j = j > 3 ? 0 : j;
		u_lcd_12864_WriteString(x,y+4,g_lcd_text[j],16);;j++; j = j > 3 ? 0 : j;
		u_lcd_12864_WriteString(x,y+6,g_lcd_text[j],16);;j++; j = j > 3 ? 0 : j;
	}
	else
	{
		u_lcd_12864_WriteString(x,y,g_lcd_text[0],16);
		u_lcd_12864_WriteString(x,y+2,g_lcd_text[1],16);
		u_lcd_12864_WriteString(x,y+4,g_lcd_text[2],16);
		u_lcd_12864_WriteString(x,y+6,g_lcd_text[3],16);
	}
}









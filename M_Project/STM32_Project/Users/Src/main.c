#include"stm32f10x.h"
#include"led.h"

void Delay(uint16_t);
/****************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/

int main(void)
{
	LED_Init();
	LED_Sets(0x00);

	while (1)
	{
		LED1(1);
		Delay(10);
		LED1(0);
		Delay(10);
	}
}
/*******************************************************************************
* Function Name  : Delay
* Description    : delay function
* Input          : c
* Output         : None
* Return         : None
*******************************************************************************/

void Delay(uint16_t c)
{
	uint16_t a,b;
	for(; c>0; c--)
		for(a=1000; a>0; a--)
			for(b=1000; b>0; b--);
}

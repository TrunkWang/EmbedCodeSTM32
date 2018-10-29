#include "u_led.h"
#include"stm32f10x.h"


#define LED_PB_IO	(GPIO_Pin_5)

void u_led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = LED_PB_IO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	/*打开LED使用的GPIO的时钟使能*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/*初始化相应的GPIO*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 //初始化GPIO_LED

}

void u_led_sets(uint8_t data)
{
	uint16_t setValue;

	setValue = GPIO_ReadOutputData(GPIOB);
	setValue &= 0x00ff;//我们使用的是16位中的高8位		
	setValue |= (uint16_t)data << 8;

	GPIO_Write(GPIOB,setValue);
}

void u_led_switch(unsigned char ON_OFF)
{
	if( ON_OFF > 0 )
	{
		GPIO_WriteBit(GPIOB, LED_PB_IO, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOB, LED_PB_IO, Bit_RESET);
	}
}

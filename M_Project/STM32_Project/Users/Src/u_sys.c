#include"u_sys.h"
#include "stm32f10x.h"


unsigned int g_tickt=0;

void u_system_init(void )
{
	//SystemInit();

	SysTick_Config(9000000);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}


unsigned int SystemTick_GetCount(void )
{
	return g_tickt;
}

void SysTick_Handler(void)
{
	g_tickt++;
}



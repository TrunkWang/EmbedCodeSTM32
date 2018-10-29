
#include <string.h>
#include <stdio.h>
#include"led.h"

#include "u_sys.h"
#include "u_uart.h"


char g_cmd_buf[DEFINE_BUF_SIZE+1];
char g_msg_buf[64];

void Delay(uint16_t c)
{
	uint16_t a,b;
	for(; c>0; c--)
		for(a=1000; a>0; a--)
			for(b=1000; b>0; b--);
}



int main(void)
{
	int recvsize=0;
	//初始化串口
	u_system_init();
	u_uart_1_init(115200);

	LED_Init();
	LED_Sets(0x00);

	while (1)
	{
		recvsize = u_uart_1_get_recv(g_cmd_buf,DEFINE_BUF_SIZE);
		if(recvsize>0)
		{
			g_cmd_buf[recvsize]=0x00;

			sprintf(g_msg_buf,"get cmd %s\n",g_cmd_buf);
			u_uart_1_sendstring(g_msg_buf,strlen(g_msg_buf));
		}
		//LED1(1);
		//Delay(10);
		//LED1(0);
		//Delay(10);
	}
}



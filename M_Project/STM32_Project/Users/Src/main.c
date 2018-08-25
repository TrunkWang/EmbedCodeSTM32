
#include <string.h>
#include <stdio.h>


#include"stm32f10x.h"

#include "u_led.h"
#include "u_sys.h"
#include "u_uart.h"
#include "u_lcd.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


unsigned char g_cmd_buf[DEFINE_BUF_SIZE+1];
unsigned char g_msg_buf[64];


QueueHandle_t queue_H;

typedef struct
{
	char ucMessageID;
	char ucData[8];
}xMessage;
xMessage msg_st;

void LedTask1(void)
{
	xMessage msg_st_T1;
	int m=0;
	int j,i;

	
	while(1)
	{
		if(uxQueueMessagesWaiting(queue_H) > 0)
		{
			xQueueReceive(queue_H,&msg_st_T1, 0);
			sprintf((char *)g_msg_buf,"msg id = %d\n",msg_st_T1.ucMessageID);
			u_uart_1_sendstring(g_msg_buf,strlen((const char *)g_msg_buf));
			
			sprintf((char *)g_msg_buf,"print_%c\n",('a'+i));
			u_uart_1_sendstring(g_msg_buf,strlen((const char *)g_msg_buf));
			u_lcd_12864_PrintString(g_msg_buf,strlen((const char *)g_msg_buf));
			
			i++;
			if(i > 25)i=0;
			
			if(m==0)
			{
				u_led_switch(0);
				m++;
			}
			else
			{
				u_led_switch(1);				
				m=0;
			}

		}
		vTaskDelay(10);
	}
}

void LedTask2(void)
{
	char rcmd[10];
	unsigned int ret;
	xMessage msg_st_T2;
	while(1)
	{
		if((ret = u_uart_1_get_recv(rcmd,10))>0)
		{
			sprintf((char *)g_msg_buf,"send queue\n");
			u_uart_1_sendstring(g_msg_buf,strlen((const char *)g_msg_buf));
			rcmd[ret] = 0x00;
			msg_st_T2.ucMessageID = 1;
			xQueueSend(queue_H, &msg_st_T2, 0);
		}
		vTaskDelay(10);
	}
}





int main(void)
{
	int i=0,j=0;
	int recvsize=0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	u_uart_1_init(115200);

	u_led_init();
	u_led_sets(0x00);
	
	u_lcd_12864_init();
	u_lcd_12864_ClearScreen();

	queue_H = xQueueCreate(5,sizeof(xMessage));
	if(queue_H != NULL)
	{
		sprintf((char *)g_msg_buf,"Queue Sucess\n");
		u_uart_1_sendstring(g_msg_buf,strlen((const char *)g_msg_buf));	
	}

	xTaskCreate(LedTask1,"led_task1",40,NULL,1,NULL);
	xTaskCreate(LedTask2,"led_task2",40,NULL,2,NULL);

	

	u_lcd_12864_PrintString("system_ready",12);

	
	vTaskStartScheduler();
	
	
	
	while (1)
	{
	}
}



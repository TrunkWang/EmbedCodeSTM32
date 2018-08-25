#include "u_uart.h"

#include <string.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"


#define UART_IN_ISR


#ifdef UART_IN_ISR

char g_u_uart_1_buf[DEFINE_BUF_SIZE];
char g_u_uart_1_buf_pos=0;

int u_uart_1_init(int uart_rate)
{
	int ret = 0;
	GPIO_InitTypeDef GPIO_InitStructure;	
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate=uart_rate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;

	USART_Init(USART1,&USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//
	USART_ClearFlag(USART1,USART_FLAG_TC);//
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
	ret = DEFINE_BUF_SIZE;
	return ret;
}


int u_uart_1_sendstring(char *str,int len)
{
	int i;
	unsigned char data;
	for(i=0;i<len;i++)
	{
		data = str[i];
		USART_SendData(USART1,data);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	}
	return i;
}


int u_uart_1_get_recv(char *str,int len)
{
	int ret=0;
	int i=0;
	unsigned int j=0;
	if(str==0)return ret;
	
	if(g_u_uart_1_buf_pos)
	{
		j = g_u_uart_1_buf_pos;
		for(i=0;i<1000;i++);
		if(j==g_u_uart_1_buf_pos)
		{
			if(len >= j)
			{
				memcpy(str,g_u_uart_1_buf,j);
				ret = j;
			}
			else
			{
				memcpy(str,g_u_uart_1_buf,len);
				ret = len;
			}
			g_u_uart_1_buf_pos = 0;
		}
	}
	return ret;
}

void USART1_IRQHandler(void )
{
  if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)//
  {
    g_u_uart_1_buf[g_u_uart_1_buf_pos]=(char)USART_ReceiveData(USART1);
		g_u_uart_1_buf_pos++;
		g_u_uart_1_buf_pos = g_u_uart_1_buf_pos & DEFINE_BUF_SIZE;
  }
}




#endif




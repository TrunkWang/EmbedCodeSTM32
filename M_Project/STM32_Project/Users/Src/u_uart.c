#include "u_uart.h"

#include "stdio.h"

#include <string.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"



#define UASRT_PA_TX GPIO_Pin_9
#define UASRT_PA_RX GPIO_Pin_10


#ifdef UART_IN_DMA


#define UART_1_DMA1_RECV_LEN	(DEFINE_BUF_SIZE)

char g_uart_1_dma1_buf[UART_1_DMA1_RECV_LEN];
unsigned int g_uart_1_recv_cnt=0;

int u_uart_1_init(int uart_rate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	USART_DeInit(USART1);
	

	GPIO_InitStructure.GPIO_Pin=UASRT_PA_TX;//TX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=UASRT_PA_RX;//RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	USART_InitStructure.USART_BaudRate=uart_rate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;

	USART_Init(USART1,&USART_InitStructure);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	USART_Cmd(USART1,ENABLE);
	
	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned int)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (unsigned int)g_uart_1_dma1_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = UART_1_DMA1_RECV_LEN;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel5,ENABLE);	
	
	return DEFINE_BUF_SIZE;
}


void MYDMA_Enable(DMA_Channel_TypeDef *DMA_CHx)
{
	DMA_Cmd(DMA_CHx,DISABLE);
	DMA_SetCurrDataCounter(DMA_CHx,UART_1_DMA1_RECV_LEN);
	DMA_Cmd(DMA1_Channel5,ENABLE);
}

void USART1_IRQHandler(void )
{
	if( USART_GetITStatus(USART1,USART_IT_IDLE) != RESET )
	{
		USART_ReceiveData(USART1);
		g_uart_1_recv_cnt = UART_1_DMA1_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel5);
		USART_ClearITPendingBit(USART1,USART_IT_IDLE);
		MYDMA_Enable(DMA1_Channel5);
	}
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
	
	if(g_uart_1_recv_cnt)
	{
		j = g_uart_1_recv_cnt;
		for(i=0;i<100;i++);
		if(j==g_uart_1_recv_cnt)
		{
			if(len >= j)
			{
				memcpy(str,g_uart_1_dma1_buf,j);
				ret = j;
			}
			else
			{
				memcpy(str,g_uart_1_dma1_buf,len);
				ret = len;
			}
			g_uart_1_recv_cnt = 0;
		}
	}
	return ret;	
}





#endif

#ifdef UART_IN_ISR


#define UART_1_BUF_POS_MASK (DEFINE_BUF_SIZE-1)

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
	

	GPIO_InitStructure.GPIO_Pin=UASRT_PA_TX;//TX
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=UASRT_PA_RX;//RX
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
		g_u_uart_1_buf_pos = g_u_uart_1_buf_pos  & UART_1_BUF_POS_MASK;
  }
}


#endif


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{

	USART_SendData(USART1,(uint8_t) ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET)
	{}

  return ch;
}



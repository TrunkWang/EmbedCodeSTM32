#ifndef _U_UART_H_
#define _U_UART_H_


//#define UART_IN_ISR
#define UART_IN_DMA


#ifdef UART_IN_DMA

#define DEFINE_BUF_SIZE   (0x80) //2 的 n 次幂

int u_uart_1_init(int uart_rate);
int u_uart_1_sendstring(char *str,int len);
int u_uart_1_get_recv(char *str,int len);


#endif

#ifdef UART_IN_ISR

#define DEFINE_BUF_SIZE  (0x80) //2 的 n 次幂

int u_uart_1_init(int uart_rate);
int u_uart_1_sendstring(char *str,int len);
int u_uart_1_get_recv(char *str,int len);

#endif


#endif

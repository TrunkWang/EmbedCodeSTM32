#ifndef _U_UART_H_
#define _U_UART_H_

#define DEFINE_BUF_SIZE  0x0F

int u_uart_1_init(int uart_rate);
int u_uart_1_sendstring(char *str,int len);
int u_uart_1_get_recv(char *str,int len);

#endif

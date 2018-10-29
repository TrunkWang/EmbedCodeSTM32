
#include <string.h>
#include <stdio.h>

#include"stm32f10x.h"

#include "u_led.h"
#include "u_sys.h"
#include "u_uart.h"
#include "u_spi.h"

#include "enc28j60.h"
#include "simple_server.h"
#include "uip.h"
#include "uip_arp.h"
#include "psock.h"


#include "timer.h"
#include "tapdev.h"

/***********************************/

//LED PB(5)

//SPI PA(12-15)
/***********************************/
#define SOCKT_LEN	(128)

char g_cmd_buf[DEFINE_BUF_SIZE];
char g_sockt_msg[SOCKT_LEN];

void main_delay(unsigned int ncount);


int main(void)
{
	int i;
  uip_ipaddr_t ipaddr;
  struct timer periodic_timer, arp_timer;
	unsigned char mymac[6] = {0x32,0x12,0x35,0x11,0x01,0x51};
	struct uip_eth_hdr *BUF=NULL;
	struct uip_conn* conns;
	uip_ipaddr_t addr_t;
	struct psock psock_t;
	
	u_system_init();

	u_uart_1_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	printf("\n----------------------------------------------\n");

	u_led_init();
	u_led_sets(0x00);

	uuu_spi_init();
  enc28j60Init(mymac);
  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);

  tapdev_init();
  uip_init();
  BUF = (struct uip_eth_hdr *)uip_getuipbuf();

  uip_ipaddr(ipaddr, 192,168,3,108);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 192,168,3,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);

  //httpd_init();


	printf("--------服务已经启动--------------------------\n");

  //simple_server(mymac);


  while(1)
  {
		
  	uip_len = tapdev_read();
    if(uip_len > 0)
    {
    	printf("___1(%d)\n",SystemTick_GetCount());
			
			printf("dest = %x.%x.%x.%x.%x.%x\n",BUF->dest.addr[0],BUF->dest.addr[1],BUF->dest.addr[2],BUF->dest.addr[3],BUF->dest.addr[4],BUF->dest.addr[5]);
    	printf("src  = %x.%x.%x.%x.%x.%x\n",BUF->src.addr[0],BUF->src.addr[1],BUF->src.addr[2],BUF->src.addr[3],BUF->src.addr[4],BUF->src.addr[5]);
    	printf("type = %d\n",BUF->type);

    	if(BUF->type == htons(UIP_ETHTYPE_IP))
    	{
    		printf("___2\n");
    		uip_arp_ipin();
    		uip_input();
			/* If the above function invocation resulted in data that
	   		should be sent out on the network, the global variable
	   		uip_len is set to a value > 0. */
			if(uip_len > 0)
			{
				uip_arp_out();
				tapdev_send();
			}
		}
		else if(BUF->type == htons(UIP_ETHTYPE_ARP))
		{
			printf("___3\n");
			uip_arp_arpin();
			printf("___4\n");
			/* If the above function invocation resulted in data that
	   		should be sent out on the network, the global variable
	   		uip_len is set to a value > 0. */
			if(uip_len > 0)
			{

				
				printf("___5\n");
				tapdev_send();
				printf("dest = %x.%x.%x.%x.%x.%x\n",BUF->dest.addr[0],BUF->dest.addr[1],BUF->dest.addr[2],BUF->dest.addr[3],BUF->dest.addr[4],BUF->dest.addr[5]);
				printf("src  = %x.%x.%x.%x.%x.%x\n",BUF->src.addr[0],BUF->src.addr[1],BUF->src.addr[2],BUF->src.addr[3],BUF->src.addr[4],BUF->src.addr[5]);
				printf("type = %d\n",BUF->type);

				printf("___6\n");
			}
		}
	}
	else if(timer_expired(&periodic_timer))
	{
	  printf("___7\n");
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++)
      {
      	uip_periodic(i);
		/* If the above function invocation resulted in data that
	  	 should be sent out on the network, the global variable
	   	uip_len is set to a value > 0. */
	   	if(uip_len > 0){
	   	uip_arp_out();

	   	tapdev_send();
	   	}
			
      }

#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++)
      {
      	uip_udp_periodic(i);
		/* If the above function invocation resulted in data that
	   	should be sent out on the network, the global variable
	   	uip_len is set to a value > 0. */
	   	if(uip_len > 0) {
	  	uip_arp_out();
	  	tapdev_send();
	  	}
      }
#endif /* UIP_UDP */

      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer))
      {	printf("___8\n");
      	timer_reset(&arp_timer);
      	uip_arp_timer();
      }
    }
  }
  return 0;

}

void main_delay(unsigned int ncount)
{
	int i=0;
	for(i=0;i<ncount;i++);
}




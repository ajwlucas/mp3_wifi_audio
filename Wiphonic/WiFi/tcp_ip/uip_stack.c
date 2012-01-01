// #include <stdio.h>
// #include <stdlib.h>
#include <string.h>
#include <print.h>

#include "uip.h"
#include "timer.h"
#include "uip_arp.h"
#include "network.h"
#include "uip_stack.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

extern unsigned char mac_addr[];
extern unsigned char local_ip[];
extern unsigned char gateway_ip[];
extern unsigned char subnet_mask[];

static struct uip_timer periodic_timer, arp_timer, self_arp_timer;

void stack_init(void)
{
	uip_ipaddr_t ipaddr;
	struct uip_eth_addr mac;
	
	mac.addr[0] = mac_addr[0];
	mac.addr[1] = mac_addr[1];
	mac.addr[2] = mac_addr[2];
	mac.addr[3] = mac_addr[3];
	mac.addr[4] = mac_addr[4];
	mac.addr[5] = mac_addr[5];
	
	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);
	timer_set(&self_arp_timer, CLOCK_SECOND * 30);
	
	uip_init();

	uip_setethaddr(mac);

	udpapp_init();
	tcp_app_init();
	
	uip_ipaddr(ipaddr, local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, gateway_ip[0],gateway_ip[1],gateway_ip[2],gateway_ip[3]);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, subnet_mask[0],subnet_mask[1],subnet_mask[2],subnet_mask[3]);
	uip_setnetmask(ipaddr);
	
}

void stack_process(chanend tx_mp3, chanend tcp_chan)
{
	int i;

	uip_len = network_read();

	if (uip_len > 0)
	{
		if (BUF->type == htons(UIP_ETHTYPE_IP))
		{
			// printstrln("ETHTYPE_IP");
			uip_arp_ipin();
			uip_input(tx_mp3, tcp_chan);
			/* If the above function invocation resulted in data that
			   should be sent out on the network, the global variable
			   uip_len is set to a value > 0. */
			if (uip_len > 0)
			{
				uip_arp_out();
				network_send();
			}
		}
		else if (BUF->type == htons(UIP_ETHTYPE_ARP))
		{
			// printstrln("ETHTYPE_ARP");
			uip_arp_arpin();
			if (uip_len > 0)
			{
				network_send();
			}
		}

	}
	else if (timer_expired(&periodic_timer))
	{
		timer_reset(&periodic_timer);

		for (i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i, tx_mp3, tcp_chan);
			if(uip_len > 0)
			{
				uip_arp_out();
				network_send();
			}
		}

		for (i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i, tx_mp3, tcp_chan);
			if (uip_len > 0)
			{
				uip_arp_out();
				network_send();
			}
		}

		// if nothing to TX and the self ARP timer expired
		// TX a broadcast ARP reply. This was implemented to
		// cause periodic TX to prevent the AP from disconnecting
		// us from the network
		if (uip_len == 0 && timer_expired(&self_arp_timer)) // 30s 
		{
			timer_reset(&self_arp_timer);
			uip_self_arp_out();
			network_send();
		}

		/* Call the ARP timer function every 10 seconds. */
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}
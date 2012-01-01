#include <xs1.h>
#include "wifi_main.h"
#include "wifi_driver.h"
#include "tcp_ip/uip_stack.h"

extern out port CE_PORT;

void wifi_ip_stack_run(chanend enable_intr, chanend wifi_intr_chan, chanend led_chan, chanend tx_mp3, chanend tcp_chan)
{
	CE_PORT <: 0;
	wifi_init();
	ReadChipInfoBlock();
	
	// Enable the interrupt trigger after reset
	enable_intr <: 1;
	
	while (zg_get_conn_state() != 1)
	{
		wifi_driver_main(wifi_intr_chan, led_chan);
	}
	
	stack_init();
	
	while (1)
	{
		stack_process(tx_mp3, tcp_chan);
		wifi_driver_main(wifi_intr_chan, led_chan);
	}
}

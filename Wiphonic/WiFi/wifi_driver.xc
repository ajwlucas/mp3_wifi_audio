// #include <xs1.h>

#include "wifi_driver.h"

extern unsigned char  zg_drv_state;
extern unsigned char  tx_ready;
extern unsigned char  rx_ready;
extern unsigned char  cnf_pending;

void wifi_driver_main(chanend intr_occured, chanend c_LED)
{
	int intr_valid = 0;
	// TX frame
	
	if (tx_ready && !cnf_pending)
	{
		zg_send();
		tx_ready = 0;
		cnf_pending = 1;
	}
	
	// Add a case here for the tx frame?
	select
	{
		case intr_occured :> int _:
			intr_valid = zg_process_isr();
			break;
		default:
			break;
	}
	
	if (intr_valid)
	{
		process_intr(c_LED);
	
		intr_valid = 0;
	}
		
	driver_process(c_LED);
	
}

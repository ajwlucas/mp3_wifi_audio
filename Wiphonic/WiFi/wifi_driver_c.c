#include <xccompat.h>
#include "wifi_driver.h"
#include "tcp_ip/uip.h"

#define IP_BUF_SIZE 1502
#define INIT_PRINT
// #define DEBUG_PRINT
// #define LOST_PRINT

unsigned char *read_buf;
unsigned short read_buf_len;
unsigned char mac_addr[6];

unsigned char zg_conn_status;

unsigned char  zg_drv_state;
unsigned char  tx_ready;
unsigned char  rx_ready;
unsigned char  cnf_pending;
unsigned char wpa_psk_key[32];

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,0,111};	// IP address of XMOS device

unsigned char gateway_ip[] = {192,168,0,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
char ssid[] = {"YOUR_SSID"};		// max 32 bytes

unsigned char security_type = 0;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
char security_passphrase[] = {"SECURITY_KEY_HERE"};	// max 64 characters

unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;


// Initialise the Wi-Fi chip and SPI protocol
// Should pass the wifi settings?
void wifi_init(void)
{
	zg_drv_state = DRV_STATE_INIT;
	zg_conn_status = 0;
	tx_ready = 0;
	rx_ready = 0;
	cnf_pending = 0;
	
	read_buf = uip_buf;
	read_buf_len = UIP_BUFSIZE;
	
	spi_init();
	spi_deselect();

	zg_chip_reset();
	zg_interrupt2_reg();
	zg_interrupt_reg(0xff, 0);
	zg_interrupt_reg(0x80|0x40, 1);
	
	ssid_len = (unsigned char)strlen(ssid);
	security_passphrase_len = (unsigned char)strlen(security_passphrase);
#ifdef INIT_PRINT
	printstrln("Finished wifi_init()");
#endif
	
}

void zg_send()
{
	read_buf[6] = 0xaa;
	read_buf[7] = 0xaa;
	read_buf[8] = 0x03;
	read_buf[9] = read_buf[10] = read_buf[11] = 0x00;
	
	WriteTXData(read_buf, read_buf_len);
	
#ifdef DEBUG_PRINT
	printstr("TX ");
	printintln(read_buf_len);
#endif
}

void zg_recv(void)
{
	// read_buf_len = (((unsigned short)read_buf[21]) << 8) | read_buf[22];
	
	unsigned short eType = 0;
	int i = 0;
	
	read_buf_len = (((unsigned short)(read_buf[20]))<<8) | (read_buf[21] & 0xFF);
	
	// Need to rearrange the received buffer from the chip into
	// a suitable ethernet packet for the UIP stack to process
	
	if (read_buf_len > 1600)
	{
		printintln(read_buf_len);
		while(1);
	}
	
	/*
	for (i=0; i < 15; i++)
	{
		printhexln(read_buf[i]);
	} */
	
	memmove(&read_buf[0], &read_buf[4], 6);
	memmove(&read_buf[6], &read_buf[10], 6);
	memmove(&read_buf[12], &read_buf[28], read_buf_len);
	
	read_buf_len += 12;

#ifdef DEBUG_PRINT
	printstr("Packet rcv: ");
	printintln(read_buf_len);
#endif
	
	/*
	printstr("dMAC: ");
	for (i=0; i < 6; i++)
	{
		printhex(read_buf[i]);
		printstr(":");
	}
	printstrln("");
	
	printstr("sMAC: ");
	for (i=6; i < 12; i++)
	{
		printhex(read_buf[i]);
		printstr(":");
	}
	printstrln(""); */

#ifdef DEBUG_PRINT
	eType = (((unsigned short)(read_buf[12]))<<8) | (read_buf[13] & 0xFF);
	printstr("eType: ");
	printhexln(eType); 
#endif
	
	/*
	zg_rx_data_ind_t* ptr = (zg_rx_data_ind_t*)&(zg_buf[3]);
	*len = ZGSTOHS( ptr->dataLen );

	memcpy(&zg_buf[0], &zg_buf[5], 6);
	memcpy(&zg_buf[6], &zg_buf[11], 6);
	memcpy(&zg_buf[12], &zg_buf[29], *len);

	*len += 12;
	*/
	
}

void zg_set_buf(unsigned char *buf, unsigned short buf_len)
{
	read_buf = buf;
	read_buf_len = buf_len;
}

void zg_write_wep_key(unsigned char cmd_buf[])
{
	// ...
}

void start_connection(unsigned char buf[])
{
	// zg_connect_req_t* cmd = (zg_connect_req_t*)&buf[3];

	// start connection to AP
	buf[0] = ZG_CMD_WT_FIFO_MGMT;
	buf[1] = ZG_MAC_TYPE_MGMT_REQ;
	buf[2] = ZG_MAC_SUBTYPE_MGMT_REQ_CONNECT;
	
	// cmd->secType = security_type;
	buf[3] = security_type;

	// cmd->ssidLen = ssid_len;
	buf[4] = ssid_len;
	
	// Zero the 32 ssid bytes
	memset(&buf[5], 0, 32);
	// Then copy the ssid 
	memcpy(&buf[5], ssid, ssid_len);
	
	// units of 100 milliseconds
	// cmd->sleepDuration = 0;
	buf[37] = 0;
	buf[38] = 0;
	
	if (wireless_mode == WIRELESS_MODE_INFRA)
	{
		// cmd->modeBss = 1;
		buf[39] = 1;
	}
	else if (wireless_mode == WIRELESS_MODE_ADHOC)
	{
		// cmd->modeBss = 2;
		buf[39] = 2;
	}
	// Reserved
	buf[40] = 0;
}

void read_psk_key(unsigned char buf[])
{
	memcpy(wpa_psk_key, ((zg_psk_calc_cnf_t*)&buf[2])->psk, 32);
}

void zg_calc_psk_key(unsigned char cmd_buf[])
{
	zg_psk_calc_req_t* cmd = (zg_psk_calc_req_t*)(cmd_buf + 3);

	cmd->configBits = 0;
	cmd->phraseLen = security_passphrase_len;
	cmd->ssidLen = ssid_len;
	cmd->reserved = 0;
	memset(cmd->ssid, 0x00, 32);
	memcpy(cmd->ssid, ssid, ssid_len);
	memset(cmd->passPhrase, 0x00, 64);
	memcpy(cmd->passPhrase, security_passphrase, security_passphrase_len);
}

void zg_write_psk_key(unsigned char cmd_buf[])
{
	zg_pmk_key_req_t* cmd = (zg_pmk_key_req_t*)(cmd_buf + 3);

	cmd->slot = 0;	// WPA/WPA2 PSK slot
	cmd->ssidLen = ssid_len;
	memset(cmd->ssid, 0x00, 32);
	memcpy(cmd->ssid, ssid, cmd->ssidLen);
	memcpy(cmd->keyData, wpa_psk_key, ZG_MAX_PMK_LEN);
}


void zg_chip_reset(void)
{
	short value;
	int loop = 0;

    /* this loop writes the necessary G2100 registers to
     * start a Hard reset. */
	do
	{
		Write16BitRegister(ZG_INDEX_ADDR_REG, ZG_RESET_REG);
		
	    value = (loop == 0u) ? (0x80) : (0x0f);
	    value = (value << 8u) | 0xff;
	    Write16BitRegister(ZG_INDEX_DATA_REG, value);
	    
	}
	while (loop++ < 1);

	Write16BitRegister(ZG_INDEX_ADDR_REG, ZG_RESET_STATUS_REG);

    /* after reset is started the host should poll a register
     * that indicates when the HW reset is complete. This
     * next loop performs that operation. */
	do
	{
		/* read the status reg to determine when the
		 * G2100 is nolonger in reset. */
		value = Read16BitRegister(ZG_INDEX_DATA_REG);
	}
	while ((value & ZG_RESET_MASK) == 0);
	

    /* After the G2100 comes out of reset the chip must complete
     * its initialization.  the following loop reads the Write
     * fifo byte count register which will be non-zero when
     * the G2100 initialization has finished. */
	do
	{
		value = Read16BitRegister(ZG_BYTE_COUNT_REG);
	}
	while (value == 0);
	
}

void zg_interrupt2_reg(void)
{
	/* write out input mask to int2 reg */
	Write16BitRegister(ZG_INTR2_REG, (unsigned short)0xFFFF);
	
	/* clear bits in int2 mask */
	Write16BitRegister(ZG_INTR2_MASK_REG, (unsigned short)0x0000);
}

void zg_interrupt_reg(int hostIntrMaskRegMask, int state)
{
	char hostIntMaskValue;
	
	/* Host Int Mask Register used to enable those events activated in Host Int Register */
    /* to cause an interrupt to the host                                                 */
	
    /* read current state of Host Interrupt Mask register */
    hostIntMaskValue = Read8BitRegister(ZG_INTR_MASK_REG);
	
    // if disabling the interrupts in the input mask
    if (state == 0)
    {
        // clear those interrupt bits
        hostIntMaskValue = (hostIntMaskValue & ~hostIntrMaskRegMask);
    }
    // else enabling interrupts in the input mask
    else
    {
        // set those interrupt bits
        hostIntMaskValue = (hostIntMaskValue & ~hostIntrMaskRegMask) | hostIntrMaskRegMask;
    }

    // write updated Host Int Mask value back to register
    Write8BitRegister(ZG_INTR_MASK_REG, hostIntMaskValue);

    // update Host Int register
    Write8BitRegister(ZG_INTR_REG, hostIntrMaskRegMask);
}

void ReadChipInfoBlock(void)
{
    int i;
    char val;
    short versionNumber = 0;

    /* read the status register to determine when it is safe to modify. */
    /* read kernel module number */
    for (i=0 ; i < 2 ; i++)
    {
        val = Read8BitRegister(0x21);

        if(i==0u)
            versionNumber = val << 8u;
        else
            versionNumber |= val & 0xff;
    }
    
    if (versionNumber != 4357)
    {
    	// Something is wrong with the connection to the device
#ifdef INIT_PRINT
    	printstrln("Error: Chip version number check failed");
#endif
    	while (1);
    }

#ifdef INIT_PRINT
    printstr("ZG2101MC Kernel Ver: ");
    printintln((int)versionNumber);
#endif
}

// Returns 1 if valid interrupt
int zg_process_isr()
{
	int intr_valid = 0;
	char intr_state = 0;
	char next_cmd = 0;
		
	char intrBits;
	char maskBits;
	unsigned int rx_byte_count;
	
    intrBits = Read8BitRegister(ZG_INTR_REG);
    maskBits = Read8BitRegister(ZG_INTR_MASK_REG);

	intr_state = ZG_INTR_ST_RD_INTR_REG;

	do
	{
		switch (intr_state)
		{
			case ZG_INTR_ST_RD_INTR_REG:
			{
				char intr_val = intrBits & maskBits;
	
				if ( (intr_val & ZG_INTR_MASK_FIFO1) == ZG_INTR_MASK_FIFO1)
				{
					Write8BitRegister(ZG_INTR_REG, ZG_INTR_MASK_FIFO1);
	
					intr_state = ZG_INTR_ST_WT_INTR_REG;
					next_cmd = ZG_BYTE_COUNT_FIFO1_REG;
				}
				else if ( (intr_val & ZG_INTR_MASK_FIFO0) == ZG_INTR_MASK_FIFO0)
				{
					Write8BitRegister(ZG_INTR_REG, ZG_INTR_MASK_FIFO0);
	
					intr_state = ZG_INTR_ST_WT_INTR_REG;
					next_cmd = ZG_BYTE_COUNT_FIFO0_REG;
				}
				else if (intr_val)
				{
					intr_state = 0;
				}
				else
				{
					intr_state = 0;
				}
	
				break;
			}
			case ZG_INTR_ST_WT_INTR_REG:
			{
				rx_byte_count = Read16BitRegister(next_cmd);
				rx_byte_count &= 0x0FFF;
				
				intr_state = ZG_INTR_ST_RD_CTRL_REG;
				break;
			}
			case ZG_INTR_ST_RD_CTRL_REG:
			{
				ReadFIFO(read_buf, rx_byte_count);
#ifdef DEBUG_FIFO				
				printstr("WiFi Read num bytes: ");
				printuintln(rx_byte_count);
#endif
				intr_valid = 1;
	
				intr_state = 0;
				break;
			}
		}
	}
	while (intr_state);
	
	return intr_valid;
}


unsigned int zg_get_rx_status()
{
	if (rx_ready) {
		rx_ready = 0;
		return read_buf_len;
	}
	else {
		return 0;
	}
}

void zg_clear_rx_status()
{
	rx_ready = 0;
}

void zg_set_tx_status(unsigned char status)
{
	tx_ready = status;
}

unsigned char zg_get_conn_state()
{
	return zg_conn_status;
}

/*
unsigned char[] zg_get_mac()
{
	return mac_addr;
} */

void process_intr(chanend LED_GREEN)
{
	switch (read_buf[0])
	{
		case ZG_MAC_TYPE_TXDATA_CONFIRM:
		{
#ifdef DEBUG_PRINT
			printstrln("TX CONFIRM");
#endif
			cnf_pending = 0;
			break;
		}
		case ZG_MAC_TYPE_MGMT_CONFIRM:
		{
			if (read_buf[2] == ZG_RESULT_SUCCESS)
			{
				switch (read_buf[1])
				{
					case ZG_MAC_SUBTYPE_MGMT_REQ_GET_PARAM:
					{
						mac_addr[0] = read_buf[6];
						mac_addr[1] = read_buf[7];
						mac_addr[2] = read_buf[8];
						mac_addr[3] = read_buf[9];
						mac_addr[4] = read_buf[10];
						mac_addr[5] = read_buf[11];
						
						zg_drv_state = DRV_STATE_SETUP_SECURITY;
#ifdef INIT_PRINT
						printstrln("WROTE MAC");
#endif
						/*
						for (int i=0; i < 6; i++)
						{
							if (i < 5)
							{
								printhex(mac_addr[i]);
								printstr(":");
							}
							else
							{
								printhexln(mac_addr[i]);
							}
						} */
						break;
					}
					case ZG_MAC_SUBTYPE_MGMT_REQ_WEP_KEY:
					{
						zg_drv_state = DRV_STATE_ENABLE_CONN_MANAGE;
						break;
					}
					case ZG_MAC_SUBTYPE_MGMT_REQ_CALC_PSK:
					{
						read_psk_key(read_buf);
						zg_drv_state = DRV_STATE_INSTALL_PSK;
						break;
					}
					case ZG_MAC_SUBTYPE_MGMT_REQ_PMK_KEY:
					{
						zg_drv_state = DRV_STATE_ENABLE_CONN_MANAGE;
						break;
					}
					case ZG_MAC_SUBTYPE_MGMT_REQ_CONNECT_MANAGE:
					{
						zg_drv_state = DRV_STATE_START_CONN;
						break;
					}
					case ZG_MAC_SUBTYPE_MGMT_REQ_CONNECT:
					{
						toggle_leds(LED_GREEN, 1);
#ifdef INIT_PRINT
						printstrln("WI-FI CONNECTED!");
#endif
						zg_conn_status = 1;	// connected
						break;
					}
					default:
						printstrln("Error: Unknown ZG_MAC_TYPE_MGMT_CONFIRM");

						break;
				}
			} // if (read_buf[2] == ZG_RESULT_SUCCESS)
			break;
		}
		case ZG_MAC_TYPE_RXDATA_INDICATE:
		{
			zg_drv_state = DRV_STATE_PROCESS_RX;
			// toggle_leds(LED_GREEN, 2);
			break;
		}
		case ZG_MAC_TYPE_MGMT_INDICATE:
		{
			switch (read_buf[1])
			{
				case ZG_MAC_SUBTYPE_MGMT_IND_DISASSOC:
				case ZG_MAC_SUBTYPE_MGMT_IND_DEAUTH:
				{
#ifdef LOST_PRINT
					printstrln("Really lost connection ;(");
#endif
					toggle_leds(LED_GREEN, 0);
					zg_conn_status = 0;	// lost connection
		
					//try to reconnect
					zg_drv_state = DRV_STATE_START_CONN;
					break;
				}
				case ZG_MAC_SUBTYPE_MGMT_IND_CONN_STATUS:
				{
					unsigned short status = (((unsigned short)(read_buf[2]))<<8) | read_buf[3];
	
					if (status == 1 || status == 5)
					{
#ifdef LOST_PRINT
						printstrln("Lost WiFi connection");
#endif
						toggle_leds(LED_GREEN, 0);
						zg_conn_status = 0;	// not connected
					}
					else if (status == 2 || status == 6)
					{
#ifdef LOST_PRINT
						printstrln("Rcv beacon in lost");
#endif
						toggle_leds(LED_GREEN, 1);
						zg_conn_status = 1;	// connected
					}
					break;
				}
				default:
					printstrln("Error: Unknown ZG_MAC_TYPE_MGMT_INDICATE");
					break;
			}
			break;
		}
	}
}

void driver_process(chanend c_LED)
{
	switch (zg_drv_state)
	{
		case DRV_STATE_INIT:
		{
			zg_drv_state = DRV_STATE_GET_MAC;
			break;
		}
		case DRV_STATE_GET_MAC:
		{
#ifdef INIT_PRINT
			printstrln("DRV_STATE_GET_MAC");
#endif
			read_buf[0] = ZG_CMD_WT_FIFO_MGMT;
			read_buf[1] = ZG_MAC_TYPE_MGMT_REQ;
			read_buf[2] = ZG_MAC_SUBTYPE_MGMT_REQ_GET_PARAM;
			read_buf[3] = 0;
			read_buf[4] = ZG_PARAM_MAC_ADDRESS;
			
			WriteFIFO(read_buf, 5);
			
			zg_drv_state = DRV_STATE_IDLE;
			break;
		}
		case DRV_STATE_SETUP_SECURITY:
		{
			switch (security_type)
			{
				case ZG_SECURITY_TYPE_NONE:
				{
#ifdef INIT_PRINT
					printstrln("No Wi-Fi security");
#endif
					zg_drv_state = DRV_STATE_ENABLE_CONN_MANAGE;
					break;
				}
				case ZG_SECURITY_TYPE_WEP:
				{
					// Install all four WEP keys on G2100
					read_buf[0] = ZG_CMD_WT_FIFO_MGMT;
					read_buf[1] = ZG_MAC_TYPE_MGMT_REQ;
					read_buf[2] = ZG_MAC_SUBTYPE_MGMT_REQ_WEP_KEY;
					
					// zg_write_wep_key(&zg_buf[3]);
					// spi_transfer(zg_buf, ZG_WEP_KEY_REQ_SIZE+3, 1);

					zg_drv_state = DRV_STATE_IDLE;
					break;
				}
				case ZG_SECURITY_TYPE_WPA:
				case ZG_SECURITY_TYPE_WPA2:
					// Initiate PSK calculation on G2100
					read_buf[0] = ZG_CMD_WT_FIFO_MGMT;
					read_buf[1] = ZG_MAC_TYPE_MGMT_REQ;
					read_buf[2] = ZG_MAC_SUBTYPE_MGMT_REQ_CALC_PSK;
					
					zg_calc_psk_key(read_buf);
					
					WriteFIFO(read_buf, ZG_PSK_CALC_REQ_SIZE+3);

					zg_drv_state = DRV_STATE_IDLE;
					break;
				default:
					break;
			}
			break;
		}
		case DRV_STATE_INSTALL_PSK:
		{
			// Install the PSK key on G2100
			read_buf[0] = ZG_CMD_WT_FIFO_MGMT;
			read_buf[1] = ZG_MAC_TYPE_MGMT_REQ;
			read_buf[2] = ZG_MAC_SUBTYPE_MGMT_REQ_PMK_KEY;
			
			zg_write_psk_key(read_buf);
			
			WriteFIFO(read_buf, ZG_PMK_KEY_REQ_SIZE+3);
			
			zg_drv_state = DRV_STATE_IDLE;
			break;
		}
		case DRV_STATE_ENABLE_CONN_MANAGE:
		{
			// enable connection manager
			read_buf[0] = ZG_CMD_WT_FIFO_MGMT;
			read_buf[1] = ZG_MAC_TYPE_MGMT_REQ;
			read_buf[2] = ZG_MAC_SUBTYPE_MGMT_REQ_CONNECT_MANAGE;
			read_buf[3] = 0x01;	// 0x01 - enable; 0x00 - disable
			read_buf[4] = 10;		// num retries to reconnect
			read_buf[5] = 0x10 | 0x02 | 0x01;	// 0x10 -	enable start and stop indication messages
												// 		 	from G2100 during reconnection
												// 0x02 -	start reconnection on receiving a deauthentication
												// 			message from the AP
												// 0x01 -	start reconnection when the missed beacon count
												// 			exceeds the threshold. uses default value of
												//			100 missed beacons if not set during initialization
			read_buf[6] = 0;
			
			WriteFIFO(read_buf, 7);
			
			// printstrln("DRV_STATE_ENABLE_CONN_MANAGE");

			zg_drv_state = DRV_STATE_IDLE;
			break;
		}
		case DRV_STATE_START_CONN:
		{
			start_connection(read_buf);
			
			WriteFIFO(read_buf, ZG_CONNECT_REQ_SIZE+3);
#ifdef INIT_PRINT
			printstrln("Attempting Wi-Fi connect...");
#endif
			
			zg_drv_state = DRV_STATE_IDLE;
			break;
		}
		case DRV_STATE_PROCESS_RX:
		{
			zg_recv();
			// toggle_leds(c_LED, 1);
			
			rx_ready = 1;
			zg_drv_state = DRV_STATE_IDLE;
			break;
		}
		case DRV_STATE_IDLE:
		{
			break;
		}
	}
}

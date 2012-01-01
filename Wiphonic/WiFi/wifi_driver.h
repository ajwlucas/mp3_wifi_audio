#ifndef WIFI_DRIVER_H_
#define WIFI_DRIVER_H_

// Includes
#include <xs1.h>

#include <print.h>
#include <string.h>

#include "zg_chip.h"
#include "spi_driver.h"
#include "zg_spi_lib.h"
#include "led_control.h"

// Defines
// #define DEBUG_FIFO

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2


void zg_chip_reset(void);
void zg_interrupt2_reg(void);
void zg_interrupt_reg(int hostIntrMaskRegMask, int state);
void ReadChipInfoBlock(void);
int zg_process_isr(void);
unsigned int zg_get_rx_status(void);
void zg_clear_rx_status(void);
void zg_set_tx_status(unsigned char status);
unsigned char zg_get_conn_state(void);

// unsigned char[] zg_get_mac(void);
void wifi_driver_main(chanend intr_occured, chanend led_chan);
void wifi_init(void);


void zg_write_wep_key(unsigned char cmd_buf[]);
void start_connection(unsigned char buf[]);
void read_psk_key(unsigned char buf[]);
void zg_calc_psk_key(unsigned char cmd_buf[]);
void zg_write_psk_key(unsigned char cmd_buf[]);
void zg_recv(void);
void zg_send(void);
void zg_set_buf(unsigned char buf[], unsigned short buf_len);
void driver_process(chanend led_chan);
void process_intr(chanend led_chan);

#endif /*WIFI_DRIVER_H_*/

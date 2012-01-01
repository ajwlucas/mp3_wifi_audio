#ifndef WIFI_MAIN_H_
#define WIFI_MAIN_H_

void wifi_ip_stack_run(chanend enable_intr, chanend wifi_intr_chan, chanend led_chan, chanend tx_mp3, chanend tcp_chan);

#endif /*WIFI_MAIN_H_*/

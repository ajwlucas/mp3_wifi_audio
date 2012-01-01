#include <string.h>
#include <print.h>

#include "uip.h"
#include "udpapp.h"
#include "tx_mp3_xcore.h"

#define STATE_INIT				0
#define STATE_LISTENING         1
#define STATE_HELLO_RECEIVED	2
#define STATE_NAME_RECEIVED		3

#define SEQ_NUM_EN	0

static struct udpapp_state s;

static int handle_tcp_conn(struct socket_app_state *s, unsigned tcp_chan);

void tcp_app_init(void)
{
	uip_listen(HTONS(13338));	// Listen on port 13338
}

/*
 * This function is called whenever an uIP event occurs
 * (e.g. when a new connection is established, new data arrives, sent
 * data is acknowledged, data needs to be retransmitted, etc.).
 */
void tcp_appcall(unsigned tcp_chan)
{
	struct socket_app_state *s = &(uip_conn->appstate);
	
	// uip_close() uip_aborted() uip_timedout()
	if (uip_connected())
	{
		PSOCK_INIT(&s->p, s->inputbuffer, sizeof(s->inputbuffer));
	}
	
	// Run the application handler with a pointer to the connection
	
	handle_tcp_conn(s, tcp_chan);
}

static int handle_tcp_conn(struct socket_app_state *s, unsigned tcp_chan)
{
	PSOCK_BEGIN(&s->p);

	// PSOCK_SEND_STR(&s->p, "0\n");
	while (1)
	{
		PSOCK_READTO(&s->p, '\n');
		// printstrln("Read");
		// printhexln(s->inputbuffer[0]);
		
		if (s->inputbuffer[0] == 0xFA)
		{
			tx_new_track(tcp_chan);
		}
		
		PSOCK_SEND_STR(&s->p, s->inputbuffer);
		// printstrln("Sent");
		memset(s->inputbuffer, 0x00, sizeof(s->inputbuffer));
	}
	
	PSOCK_CLOSE(&s->p);

	PSOCK_END(&s->p);
}

void udpapp_init(void)
{
	uip_ipaddr_t addr;
	struct uip_udp_conn *c;

	uip_ipaddr(&addr, 192,168,0,101);	// 112
	c = uip_udp_new(&addr, HTONS(0));
	if(c != NULL) {
		uip_udp_bind(c, HTONS(13337));
	}

	s.state = STATE_INIT;

	PT_INIT(&s.pt);
}

static PT_THREAD(handle_connection(unsigned tx_mp3))
{
	PT_BEGIN(&s.pt);

	s.state = STATE_LISTENING;
	static int count = 0;
	int num;
	
	PT_WAIT_UNTIL(&s.pt, uip_newdata());
	
	if (uip_newdata())
	{
#if SEQ_NUM_EN
		char* data_start = uip_appdata;
		data_start += 4;
		
		num = tx_mp3_xcore(data_start, uip_len, tx_mp3);
#else
		
		num = tx_mp3_xcore((char*)uip_appdata, uip_len, tx_mp3);
#endif	
		count++;
		// printintln(count);
		// printintln(uip_len);

		uip_flags &= (~UIP_NEWDATA);
		
	}

	s.state = STATE_INIT;

	PT_END(&s.pt);
}

void udpapp_appcall(unsigned tx_mp3)
{
	handle_connection(tx_mp3);
}

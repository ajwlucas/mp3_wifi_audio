/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/

#include <xs1.h>
#include <print.h>
#include <platform.h>

// MP3 decoder
#include "MP3_decoder/decode_main.h"
#include "MP3_decoder/pcm_buffer.h"
#include "MP3_decoder/dataServer.h"
#include "MP3_decoder/dataClient.h"
// PWM audio
#include "PWM/classD.h"
// Wi-Fi driver 
#include "WiFi/wifi_main.h"

#define UDP_FIFO_DEBUG 0

#define MP3_BUF_SIZE 15000

#define BUFFER_NUM_FRAMES (unsigned int)10	// Needs to fit within MP3_BUF_SIZE

//////////////////// PORT DEFINES
on stdcore[0] : out port 				LEFT	= XS1_PORT_1M;	// 
on stdcore[0] : out port 				RIGHT	= XS1_PORT_1O;	//  
on stdcore[0] : out port 				TEST	= XS1_PORT_1A;	// 

on stdcore[2] : out buffered port:8 	spi_mosi = XS1_PORT_1F;	// F 13 -> 11
on stdcore[2] : out buffered port:8 	spi_sclk = XS1_PORT_1E;	// E 12 -> 13
on stdcore[2] : in buffered port:8 		spi_miso = XS1_PORT_1H;	// H 23 -> 12
on stdcore[2] : out port 				spi_ss = XS1_PORT_1A;	// A 0 -> 10

on stdcore[2] : in port					INT_PORT = XS1_PORT_1G;	// G 22 -> 2
on stdcore[2] : out port				CE_PORT	= XS1_PORT_1B;	// Pull low

on stdcore[0] : in port					RESET_BUTTON = XS1_PORT_4D;

// LEDs
out port LED_GREEN = PORT_CLOCKLED_SELG;
out port LED_RED = PORT_CLOCKLED_SELR;
out port LED0	= PORT_CLOCKLED_0;
out port LED1 	= PORT_CLOCKLED_1;
out port LED2	= PORT_CLOCKLED_2;

// SPI clocks
// (1) continuous clock
// (2) software initiated clock ticks aligned to (1)
on stdcore[2] : clock blk1 = XS1_CLKBLK_1;
on stdcore[2] : clock blk2 = XS1_CLKBLK_2;

// PWM clock
on stdcore[0] : clock REF_CLOCK = XS1_CLKBLK_3; 

void chipReset(void)
{
  unsigned x;
  
  read_sswitch_reg(get_core_id(), 6, x);
  write_sswitch_reg(get_core_id(), 6, x);
}

void intr_monitor(chanend wifi_intr_chan, chanend enable_intr)
{
	int enable = 0;
	
	while (1)
	{
		select
		{
			case enable_intr :> enable:
				break;
			default:
				break;
		}
			
		if (enable)
		{
			INT_PORT when pinsneq(1) :> void;
			// printstrln("INTERRUPT");
			// Signal interrupt to main driver process
			wifi_intr_chan <: 1;
			// Wait for the interrupt to clear
			INT_PORT when pinsneq(0) :> void;
		}
	}
}

int main(void)
{
	chan pcmChan;
	// chan debugChan;
	chan fifoChan;
	streaming chan pwmChan;
	// chan rxChan;
	
	chan wifi_intr_chan;
	chan enable_intr;
	chan led_chan;
	chan tx_mp3;
	chan rx_mp3;
	
	chan tcp_chan;
	chan mp3Reset;
	
	par
	{
		// decoderMain()
		// The MP3 decoder. Receives MP3 data on rxChan and outputs PCM samples on pcmChan
		on stdcore[3] :
		{
			while (1)
			{
				decoderMain(pcmChan, rx_mp3, mp3Reset);
			}
		}
		// 
		// txData()
		// Sends compressed MP3 data from this core to the decoder for testing. See mp3_data.h
		// on stdcore[0] : txData(rxChan);
		
		// pcmBufRx()
		// Receives the PCM samples from the MP3 decoder on pcmChan and reorders them into play
		// order. Sends to FIFO() via fifoChan in blocks of 64 samples (LRLRLR.. stereo)
		on stdcore[1] : pcmBufRx(pcmChan, fifoChan);
		
		// FIFO()
		// A leaky bucket that buffers PCM samples from the decoder so they can be output to the
		// PWM at constant rate
		on stdcore[1] : FIFO(fifoChan, pwmChan);
		// on stdcore[2] : debugRx(debugChan);
		
		// The PWM implementation. Receives samples on pwmChan
		on stdcore[0] :
		{	
#if 1
			configure_clock_ref(REF_CLOCK, 0);
			configure_out_port(LEFT, REF_CLOCK, 0);
			configure_out_port(RIGHT, REF_CLOCK, 0);
			start_clock(REF_CLOCK);
			
			audioManager(pwmChan, LEFT, RIGHT, TEST);
#endif
		}
		
		on stdcore[2] : intr_monitor(wifi_intr_chan, enable_intr);
		
		on stdcore[2] :
		{
			// CE_PORT <: 1;
			wifi_ip_stack_run(enable_intr, wifi_intr_chan, led_chan, tx_mp3, tcp_chan);
		}
		
		on stdcore[0] :			// MP3 frame FIFO
		{
			unsigned char mp3_buffer[MP3_BUF_SIZE];
			int cur_length = 0;
			unsigned char msb;
			unsigned char lsb;
			int p_read = 0;
			int p_write = 0;
			int byte_count = 0;
			unsigned int frame_count = 0;
			int error_check = 0;
			
			int reset = 0;
			
			while (1)
			{
				// Reset
				cur_length = 0;
				p_read = 0;
				p_write = 0;
				byte_count = 0;
				frame_count = 0;
				reset = 0;
				
				while (1)
				{
					select
					{
						case tcp_chan :> reset :
						{
							break; // RESET
						}
						case tx_mp3 :> msb:
						{
							tx_mp3 :> lsb;
							
							byte_count = 0;		

							// Store the length in the buffer as big endian
							mp3_buffer[p_write] = msb;
							
							p_write++;
							if (p_write >= MP3_BUF_SIZE) p_write = 0;
							
							mp3_buffer[p_write] = lsb;
							p_write++;
							if (p_write >= MP3_BUF_SIZE) p_write = 0;
							
							cur_length = ((msb << 8)|(lsb&0xFF));
							
							if (cur_length > 1500)
							{
								printstrln("Length broke");
							}
							
							if (cur_length == 0)
							{
								printstrln("No MP$$$ data!");
								while (1);
							}
							
							while (1) // Output one frame's worth of bytes to the MP3 decoder
							{
								
								select
								{
									case tx_mp3 :> mp3_buffer[p_write]:	// At the same time, input from UDP
									{
										p_write++;
										if (p_write >= MP3_BUF_SIZE) p_write = 0;
										byte_count++;
										break;
									}
									default:
									{
										/*
										if (frame_count >= BUFFER_NUM_FRAMES)
										{
											if (p_read + 1 != p_write)
											{
												rx_mp3 <: mp3_buffer[p_read];
												p_read++;
												if (p_read >= MP3_BUF_SIZE) p_read = 0;
											}
											else
											{
												// printstrln("Underflow1");
											}
										} */
										break; 
									}
										
								}
								if (byte_count == cur_length)
								{
									frame_count++;
									
									if (UDP_FIFO_DEBUG)
									{
										printstr("frame_count: ");
										printintln(frame_count);
									}
									break;
								}
							}
							// printstr("Num rx: ");
							// printintln(cur_length);
								
							break;
						}
						default:
						{
							if (frame_count >= BUFFER_NUM_FRAMES)
							{
								if (p_read + 1 != p_write)	// There are bytes in the buffer to decode
								{
									rx_mp3 <: mp3_buffer[p_read];
									p_read++;
									if (p_read >= MP3_BUF_SIZE) p_read = 0;
								}
								else
								{
									// printstrln("Underflow2");
								}
							}
							else if (UDP_FIFO_DEBUG && frame_count == 0 && byte_count > 0)
							{
								error_check++;
								if (error_check >= BUFFER_NUM_FRAMES)
								{
									printstrln("BUFFER_NUM_FRAMES");
								}

							}
							break;
						}
					}
					if (reset)
					{
						// Send a reset TCP confirm
						tcp_chan <: 1;
						mp3Reset <: 1;
						break;
					}
				}
			}
			
			
		}
		
		on stdcore[0] :		// LED control thread
		{
			int led_on = 0;
			int connecting = 1;
			timer t;
			unsigned int time;
			int bit = 0;
			LED_GREEN <: 0;
			LED_RED <: 0;
			LED0 <: 0xF;
			LED1 <: 0xF;
			LED2 <: 0xF;
			
			t :> time;
			while (1)
			{
				select
				{
					case led_chan :> led_on :
					{
						if (led_on == 1)
						{
							connecting = 0;
							LED_GREEN <: 1;
						}
						else
						{
							connecting = 1;
							LED_GREEN <: 0;
						}
						break;
					}
					default:
					{
						if (connecting)
						{
							time += 30000000;
							t when timerafter(time) :> void;
							LED_GREEN <: bit;
							
							bit = !bit;
						}
						break;
					}
				}
				
			}
		}
		on stdcore[0] :	// Reset chip on button press
		{
			int val;
			while (1)
			{
				RESET_BUTTON when pinsneq(0xf) :> void;
				
				chipReset();
			}
		}
	}
	
	return 0;
}

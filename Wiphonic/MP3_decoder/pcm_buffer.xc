/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/
#include <platform.h>
#include <print.h>
// #include <stdio.h>
#include "pcm_buffer.h"
#include "../PWM/pwmDefines.h"

extern int sineWave1000[1000];

void FIFO(chanend pcmIn, streaming chanend pwmOut)
{
	unsigned short buffer[2049];
	int write_idx = 0;
	int read_idx = 0;
	
	timer t;
	unsigned long time;

	unsigned short sample_in;
	unsigned short sample_out;
	
	int sampleNo = 0;
	int left = 0;
	
	// Infinite loop for testing timing
	
	/*
	t :> time;
	while(1)
	{
		pwmOut <: sample;
		
		time += (PWM_PERIOD_44100 * 2);
		t when timerafter(time) :> void;
	} */
	
	// The actual FIFO implementation
	
	/*
	t :> time;
	while(1)
	{
		select
		{
			case t when timerafter(time) :> void:
					pwmOut <: 0x0000;
					time += (PWM_PERIOD_44100 * 4);

					read_idx++;
					break;
			default:
				break;
		}
	} */
	
	sample_out = 0x0000;
	
	while(1)
	{
		for (int i=0; i < 1024; i++)
		{
			pcmIn :> sample_in;
			buffer[write_idx] = sample_in;
		/*
			printint(write_idx);
			printstr(" in: ");
			printintln(sample_in);
		*/
			write_idx++;
		}
		sample_out = buffer[read_idx];
		
		t :> time;
		while(1)
		{
			select
			{
				case t when timerafter(time) :> void:
					if (read_idx + 1 != write_idx)
					{
						pwmOut <: sample_out;
					/*
						printint(read_idx);
						printstr(" out: ");
						printintln(sample_out);
					*/
			          
						time += (PWM_PERIOD_44100 * 2);
	
						read_idx++;
						if (read_idx >= 2048)
						{
							read_idx = 0;
						}
						sample_out = buffer[read_idx];
					}
					else
					{
						/*
						if (read_idx == 0) 
						{
							buffer[read_idx] = buffer[2048];
						}
						else
						{
							buffer[read_idx] = buffer[read_idx-1];
						}
						pwmOut <: buffer[read_idx];
						
						read_idx++;
						
						if (read_idx >= 2048)
						{
							read_idx = 0;
						}
						
						time += (PWM_PERIOD_44100 * 2); */
						
						/* pwmOut <: 0x0000;
												
						read_idx++;
						if (read_idx >= 2048)
						{
							read_idx = 0;
						} */
						
						pwmOut <: buffer[read_idx];
						
						time += (PWM_PERIOD_44100 * 2);
						// printstrln("\nFIFO Overflow");
					}
					break;
				default:
					/*
					for (int i=0; i < 10; i++)
					{
						if (read_idx != write_idx)
						{
							select
							{
								case pcmIn :> sample_in:
									buffer[write_idx] = sample_in;
								
									printint(write_idx);
									printstr(" in: ");
									printintln(sample_in);
								
									write_idx++;
									if (write_idx >= 2048)
									{
										write_idx = 0;
									}
									break;
								default:
									break;
							}
						}
					} */
					
					if (read_idx != write_idx)
					{
						select
						{
						case pcmIn :> sample_in:
								buffer[write_idx] = sample_in;
							/*
								printint(write_idx);
								printstr(" in: ");
								printintln(sample_in);
							*/
								write_idx++;
								if (write_idx >= 2048)
								{
									write_idx = 0;
								}
							break;
						default:
							break;
						}
					}
					else
					{
						/*
						if (write_idx == 0) 
						{
							buffer[write_idx] = buffer[2048];
						}
						else
						{
							buffer[write_idx] = buffer[write_idx-1];
						}
						write_idx++;
						if (write_idx >= 2048)
						{
							write_idx = 0;
						} */
					}
					break; 
			}
		}
	}
	
	/*
 	select
	{
		case pcmIn :> sample:
			t :> time;
			while(1)
			{
				select
				{
					case t when timerafter(time) :> void:
						if (read_idx + 1 != write_idx)
						{
							pwmOut <: buffer[read_idx];
							time += (PWM_PERIOD_44100 * 8);
							read_idx++;
							if (read_idx >= 2048)
							{
								read_idx = 0;
							}
						}
						break;
					case pcmIn :> sample:
						if (read_idx != write_idx)
						{
							buffer[write_idx] = sample;
							write_idx++;
							if (write_idx >= 2048)
							{
								write_idx = 0;
							}
						}
						break;
					default:
						break;
				}
			}
			break;
	} */
}

/******************************************************************************************
* void pcmBufRx(chanend pcmChan, chanend fifoOut)
* 
* Receives 16 bit PCM samples sent over a channel from the polyphase filter. 
* Samples aren't received in play order thus are sent with an index which allows
* them to be sorted in an array and passed onto PWM in the correct order after buffering
* 
******************************************************************************************/
void pcmBufRx(chanend pcmChan, chanend fifoOut)
{
	// short pcmBufL[32];
	// short pcmBufR[32];
	short pcmBuf[64];
	// int bufIdx = 0;
	// int leftNext = 1;
	
	int word, pcmIdx = 0;
	signed short pcmSample;
	
	while (1)
	{
			pcmChan :> word;	
			pcmSample = word >> 16;
			pcmIdx = word & 0xffff;
		
			if (pcmIdx > 64) // Not an output sample
			{
				if (pcmSample == 0xffff)
				{
					// A new MP3 frame -> reset buffer index
					// bufIdx = 0;
					// printintln(-256);
				}
				else
				{
					// A bad frame -> flush buffer with zeros
					for (int i = 0; i < 64; i++)
					{
						pcmBuf[i] = 0;
					}
				}
			}
			else
			{
				// Store the sample
				pcmBuf[pcmIdx] = pcmSample;
			}
		
			
			if (pcmIdx == 35) // The last sample to be output by the MP3 decoder
			{
				// Output all 64 samples in the correct order
				for (int i=0; i < 64; i++)
				{
					fifoOut <: pcmBuf[i];
					
					// Put a print here to see the PCM samples in the correct order
					
					/*
					// Testing right samples
					if ((i & 0x01) == 1)
					{
						if (pcmBuf[i] != 0)
						{
							printint(i);
							printstr(": ");
							printintln(pcmBuf[i]);
						}
					} */
					/*
					printint(i);
					printstr(": ");
					printintln(pcmBuf[i]);
					*/
				}	
			} 
	}
}

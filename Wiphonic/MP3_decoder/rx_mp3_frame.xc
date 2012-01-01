#include <xs1.h>
#include <print.h>
#include "rx_mp3_frame.h"

int RxNewFrame(unsigned char readBuf[], int size, chanend rx_mp3, chanend mp3Reset)
{
	int frame_len = 0;
	unsigned char MSB;
	unsigned char LSB;
	int index = 0;

	select
	{
		case mp3Reset :> int _ :
		{
			// printstrln("MP3 RESET");
			return 0;
		}
		case rx_mp3 :> MSB :
		{
			while (1)
			{
				select
				{
					case mp3Reset :> int _ :
					{
						// printstrln("MP3 RESET");
						return 0;
					}
					case rx_mp3 :> LSB :
					{
						break;
					}
				}
				break;
			}
			break;
		}
	}
	
	frame_len = ((MSB << 8)|(LSB&0xFF));
	
	if (frame_len > 1500)
	{
		return 0;
	}
	
	do
	{
#pragma ordered
		select
		{
			case mp3Reset :> int _ :
			{
				// printstrln("MP3 RESET");
				return 0;
			}
			case rx_mp3 :> readBuf[index]:
			{
				index++;
				break;
			}
		}
	} while (index < frame_len);
	
	// printintln(frame_len);
	
	return frame_len;

}
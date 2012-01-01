#include <xs1.h>
#include <print.h>

#include "tx_mp3_xcore.h"

int tx_mp3_xcore(char data[], int length, chanend c)
{
	unsigned char msb = (unsigned char)(length >> 8);	// MSB
	unsigned char lsb = (unsigned char)(length);		// LSB
	
	c <: msb;
	c <: lsb;
	
	for (int i=0; i < length; i++)
	{
		c <: data[i];
	}
	return 0;
}

int tx_new_track(chanend c)
{
	int receive;
	c <: (int)1;
	c :> receive;
	
	return receive;
}
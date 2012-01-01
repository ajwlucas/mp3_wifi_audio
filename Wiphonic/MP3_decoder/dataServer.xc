/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/

#include <platform.h>

#include "dataServer.h"
#include "mp3_data.h"
// #include <print.h>

void txData(chanend dataChan)
{
	int bufIdx = 0;
	int bytesLeft = sizeof(mp3);
	
	while (1)
	{
		int numBytes;
		// First expect the number of bytes to send
		dataChan :> numBytes;
		if (numBytes > bytesLeft)
		{
			numBytes = bytesLeft;
		}
		
		// Send the number you are going to send 
		dataChan <: numBytes;
		
		// Then send the bytes
		for (int i=0; i < numBytes; i++)
		{
			dataChan <: mp3[bufIdx];
			// printhexln(mp3[bufIdx]);
			bufIdx++;
			bytesLeft--;
		}
		
		if (bytesLeft == 0)
		{
			bytesLeft = sizeof(mp3);
			bufIdx = 0;
		}
		
	}
}

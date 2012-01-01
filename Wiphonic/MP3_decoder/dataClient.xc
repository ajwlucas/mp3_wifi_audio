/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/

#include <platform.h>
#include "dataClient.h"

int rxData(chanend data, int numBytes, unsigned char buffer[])
{
	int bytesExpected;
	
	data <: numBytes;
	data :> bytesExpected;
	
	if (bytesExpected > 0)
	{
		for (int i=0; i < bytesExpected; i++)
		{
			data :> buffer[i];
		}
	}
	
	return bytesExpected;
}

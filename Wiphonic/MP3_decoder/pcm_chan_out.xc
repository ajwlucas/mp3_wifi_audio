/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/
#include <platform.h>
#include <print.h>
#include "pcm_chan_out.h"

void OutputToPCMBuf(short pcmSample, short index, chanend pcmChan)
{
	int word = (pcmSample << 16) | index;
	pcmChan <: word;
}

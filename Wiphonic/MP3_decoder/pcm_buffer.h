/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/

#ifndef PCM_BUFFER_H_
#define PCM_BUFFER_H_

void pcmBufRx(chanend pcmChan, chanend fifoOut);
void FIFO(chanend pcmIn, streaming chanend pwmOut);

#endif /*PCM_BUFFER_H_*/

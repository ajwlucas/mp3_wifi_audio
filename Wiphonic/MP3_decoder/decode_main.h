/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/
#ifndef DECODE_MAIN_H_
#define DECODE_MAIN_H_

// int FillReadBuffer(unsigned char *readBuf, unsigned char *readPtr, int bufSize, int bytesLeft, chanend rxChan);
int decoderMain(chanend pcmChan, chanend rxData, chanend mp3Reset);


#endif /*DECODE_MAIN_H_*/

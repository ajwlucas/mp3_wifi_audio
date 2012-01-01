/**************************************************************************************
 * Author: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <print.h>

// MP3 Decoder
#include "mp3dec.h"
// XC linker prototypes
#include "decode_main.h"
#include "pcm_chan_out.h"
#include "rx_mp3_frame.h"
#include "dataClient.h"

#define READBUF_SIZE (MAX_NGRAN * MAX_NSAMP * 2)

static int FillReadBuffer(unsigned char *readBuf, unsigned char *readPtr, int bufSize, int bytesLeft, chanend rxChan)
{
	int nRead;

	/* move last, small chunk from end of buffer to start, then fill with new data */
	memmove(readBuf, readPtr, bytesLeft);
	nRead = rxData(rxChan, bufSize - bytesLeft, readBuf + bytesLeft);
	/* zero-pad to avoid finding false sync word after last frame (from old data in readBuf) */
	if (nRead < bufSize - bytesLeft)
		memset(readBuf + bytesLeft + nRead, 0, bufSize - bytesLeft - nRead);	

	return nRead;
}

int decoderMain(chanend pcmChan, chanend rxChan, chanend mp3Reset)
{
	int bytesLeft, nRead, err, offset, outOfData, eofReached, nFrames;
	int reset;
	unsigned char readBuf[READBUF_SIZE], *readPtr;
	// unsigned char mp3_data
	// short outBuf[MAX_NCHAN * MAX_NGRAN * MAX_NSAMP];
	// short outBuf[256];

	MP3FrameInfo mp3FrameInfo;
	HMP3Decoder hMP3Decoder;
	
	if ( (hMP3Decoder = MP3InitDecoder()) == 0 )
	{
		return -2;
		// puts("Init died");
	}
	
	bytesLeft = 0;
	outOfData = 0;
	eofReached = 0;
	readPtr = readBuf;
	nRead = 0;

	nFrames = 0;
		
	do
	{
		
		bytesLeft = RxNewFrame(readBuf, READBUF_SIZE, rxChan, mp3Reset);
		if (bytesLeft == 0)
		{
			break;
		}
		readPtr = readBuf;
		
		/* decode one MP3 frame - if offset < 0 then bytesLeft was less than a full frame */
		
		err = MP3Decode(hMP3Decoder, &readPtr, &bytesLeft, NULL, 1, pcmChan);
		
		// err = MP3Decode(hMP3Decoder, &readPtr, &bytesLeft, NULL, 0, pcmChan);
		nFrames++;
		// printintln(nFrames);
		
		// Need to flush the PCM buffer for next frame decode
		// By outputting all ones to the channel
		// OutputToPCMBuf(0xffff, 0xffff, pcmChan);
		
		if (err)
		{
			/* error occurred */
			switch (err)
			{
			case ERR_MP3_INDATA_UNDERFLOW:
				printstrln("HERE");
				outOfData = 1;
				break;
			case ERR_MP3_MAINDATA_UNDERFLOW:
				/* do nothing - next call to decode will provide more mainData */
				break;
			case ERR_MP3_FREE_BITRATE_SYNC:
			default:
				outOfData = 1;
				// printintln(err);
				// printstrln("\nHERE2");
				// while (1);
				break;
			}
		}
		else 
		{
			/* no error */
			MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
		}

	} while (!outOfData);
	
	// printstr("FINISHED: ");
	// printintln(err);
	
	MP3FreeDecoder(hMP3Decoder);
	// printDebug(1, debugChan);
	
	return 0;
}

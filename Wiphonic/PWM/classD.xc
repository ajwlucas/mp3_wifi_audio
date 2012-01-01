#include "global.h"

#ifdef CLASSD_OUTPUT

#include <syscall.h>
#include <platform.h>
#include <xs1.h>
#include <xclib.h>
#include <print.h>
#include "pwmDefines.h"
#include "classD.h"
// #include "audioManager.h"

#define ENABLE_AUDIO_OUT 1

// Share these attributes across the PWM threads
int attribute [NUM_ATTR_ELEMS] = { 44100,                // SAMPLE_RATE_ELEM
                                   PWM_PERIOD_44100,     // PWM_PERIOD_ELEM
                                   MS_DELAY_WHOLE_44100, // MS_DELAY_WHOLE_ELEM
                                   MS_DELAY_FRAC_44100,  // MS_DELAY_FRAC_ELEM
                                   0};               // VOLUME_ELEM

// The interpolated samples are passed to the pwm thread through these Fifos for left and right data
int pwmSampleFifoL     [PWM_FIFO_SIZE];
int pwmSampleFifoLCheck[PWM_FIFO_SIZE];
int pwmSampleFifoR     [PWM_FIFO_SIZE];
int pwmSampleFifoRCheck[PWM_FIFO_SIZE];

// Function prototypes
void audioDac(streaming chanend c_dacL, streaming chanend pwmTimingLChan,
               int pwmSampleFifo[PWM_FIFO_SIZE],
               int pwmSampleFifoCheck[PWM_FIFO_SIZE],
               int left,
               out port? testPort);
void pwmThread (out port LEFT, out port RIGHT, streaming chanend pwmTimingLChan, streaming chanend pwmTimingRChan);

/////////////////////////////////////
void audioManager(
   /* streaming chanend cSOFGen, streaming chanend cAudioMixer, streaming chanend cAudioDFU,*/ streaming chanend PCM_IN,
    out port LEFT, out port RIGHT, out port TEST)
{
  streaming chan c_sampleL;       // raw channel samples
  streaming chan c_sampleR;       // raw channel samples
  streaming chan pwmTimingLChan;  // timing info from pwm thread to interpolation thread: start interpolating next sample
  streaming chan pwmTimingRChan;  // timing info from pwm thread to interpolation thread: start interpolating next sample

  unsigned sample;
  int cmd;
  int sampleLeftNext = TRUE;

  // First spawn interpolation and PWM threads
  par
  {
	  // c_sampleL is c_dac in audioDac()
      audioDac(c_sampleL, pwmTimingLChan, pwmSampleFifoL, pwmSampleFifoLCheck, 1, null); // null );
      audioDac(c_sampleR, pwmTimingRChan, pwmSampleFifoR, pwmSampleFifoRCheck, 0, TEST); // audioports.loFetGateL); // null );
      pwmThread(LEFT, RIGHT, pwmTimingLChan, pwmTimingRChan);

    {
    	
      // Set volume
	  int volume;
	  int volHi, volLo;
	  int volumeRaw = 0x100;
	    
	  volume = volumeRaw << (30-8); // 0x40000000 for 0x100 (max vol)
	  {volHi, volLo} = mac (volume, volume, 0, 0);
	  volume = volHi << 2;
	  {volHi, volLo} = mac (volume, volume, 0, 0);
	  volume = (volHi << 3)-1;
	  attribute[VOLUME_ELEM] = volume;                   // vol^4


      
      while (1)
      {
        // ensure that the SOF takes priority in the select statement
        #pragma ordered
        select
        {
	
          ///////////////////////////
          // get the audio samples
          case PCM_IN :> sample:

        	sample <<= 16;
        		  

            #if SAMPLE_SOURCE == SINE_USB_SPLIT
              // Inject a sine wave into the stream
              sample = sineWave1000[sampleNo];
              sampleNo+=SAMPLE_INC;
              if (sampleNo >= 1000)
              {
                sampleNo -= 1000;
              }
            #endif

            // The sample channel is used to transmit a stop signal
            // Avoid this value being used by a real sample by replacing it with a similar value
            if (sample == DUMMY_SAMPLE_STOP)
            {
              sample = DUMMY_SAMPLE_REPLACE;
            }
            
            if (sampleLeftNext == TRUE)
            {

            	// sample <<= 16;
            	
              c_sampleL <: sample;

              sampleLeftNext = FALSE;
            } else
            {
            	
              c_sampleR <: sample;
              sampleLeftNext = TRUE;
            }
            break;

       
        } // select
      } // while(1)
    } // This thread
  } // par
}


#endif // CLASSD_OUTPUT

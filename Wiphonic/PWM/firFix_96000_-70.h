/**
 * Module:  module_ipod_support
 * Version: 1v0
 * Build:   7097304a351e95b4756e7a7125bb030ce017b929
 * File:    firFix_96000_-70.h
 *
 * The copyrights, all other intellectual and industrial 
 * property rights are retained by XMOS and/or its licensors. 
 * Terms and conditions covering the use of this code can
 * be found in the Xmos End User License Agreement.
 *
 * Copyright XMOS Ltd 2009
 *
 * In the case where this code is a modification of existing code
 * under a separate license, the separate license terms are shown
 * below. The modifications to the code are still covered by the 
 * copyright notice above.
 *
 **/                                   
// These coefficients are scaled to allow headroom in the result.
// They are in 4q28 format.

#define s1_NUM_COEFF 38
#define s1_UPSAMPLING_RATE 2

int s1FirCoeff[s1_NUM_COEFF] = {
    0x000c2931 ,
    0x001645a6 ,
    0xfffdb21c ,
    0xffdbb2dc ,
    0xfffcf526 ,
    0x003e5240 ,
    0x00101ec4 ,
    0xff9c855f ,
    0xffd784d2 ,
    0x0097399f ,
    0x00527091 ,
    0xff1f071b ,
    0xff650256 ,
    0x0153295a ,
    0x01243600 ,
    0xfdd30235 ,
    0xfd839a56 ,
    0x05056396 ,
    0x0e175bfb ,
    0x0e175bfb ,
    0x05056396 ,
    0xfd839a56 ,
    0xfdd30235 ,
    0x01243600 ,
    0x0153295a ,
    0xff650256 ,
    0xff1f071b ,
    0x00527091 ,
    0x0097399f ,
    0xffd784d2 ,
    0xff9c855f ,
    0x00101ec4 ,
    0x003e5240 ,
    0xfffcf526 ,
    0xffdbb2dc ,
    0xfffdb21c ,
    0x001645a6 ,
    0x000c2931
};


// With inputs in the range [-1,+1], the maximum output with these coefficients is:
//        3.7234471498853896e+00
//
// Note that this not scaled, so 1.0 corresponds to 1q31
// Note also that both the positive and negative values can occur


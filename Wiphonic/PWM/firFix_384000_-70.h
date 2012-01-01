/**
 * Module:  module_ipod_support
 * Version: 1v0
 * Build:   7097304a351e95b4756e7a7125bb030ce017b929
 * File:    firFix_384000_-70.h
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

#define s2_NUM_COEFF 22
#define s2_UPSAMPLING_RATE 4

int s2FirCoeff[s2_NUM_COEFF] = {
    0x000a328e ,
    0x00002ae5 ,
    0xffc2d2f4 ,
    0xff4012cd ,
    0xfeaf8a75 ,
    0xfea7d024 ,
    0xffe80a67 ,
    0x02da1bc1 ,
    0x07197127 ,
    0x0b62b681 ,
    0x0e17c9f5 ,
    0x0e17c9f5 ,
    0x0b62b681 ,
    0x07197127 ,
    0x02da1bc1 ,
    0xffe80a67 ,
    0xfea7d024 ,
    0xfeaf8a75 ,
    0xff4012cd ,
    0xffc2d2f4 ,
    0x00002ae5 ,
    0x000a328e
};


// With inputs in the range [-1,+1], the maximum output with these coefficients is:
//        4.9014283351312589e+00
//
// Note that this not scaled, so 1.0 corresponds to 1q31
// Note also that both the positive and negative values can occur


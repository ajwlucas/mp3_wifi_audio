/**
 * Module:  app_ipod_dock
 * Version: 1v0
 * Build:   7097304a351e95b4756e7a7125bb030ce017b929
 * File:    global.h
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
#ifndef GLOBAL_H_
#define GLOBAL_H_

#define CLASSD_OUTPUT

//#define ENFORCE_FAST_MODE

// ---- Release mode: ----
// Debug statements removed, streaming channels used, etc.
// Defined when calling makefile with RELEASE=1

// ---- Audio Output mode: ----
// OUTPUT is now defined via makefile option (default is i2s)
// Options are:
// OUTPUT=I2S, OUTPUT=SPDIF, OUTPUT=I2SSPDIF, OUTPUT=CLASSD

// Number of USB clients for the USB stack
#define NUM_CTRL_CHANENDS 3

// ----------------------------------------------------------------------


#ifdef RELEASE
#define USE_STREAMING_CHANNELS
#undef VERBOSE
#else
#undef USE_STREAMING_CHANNELS
#define VERBOSE
#endif

#include <xs1.h>

// ------------------------------------------------------------------------------------------------------------
#include "print.h"
#ifdef VERBOSE
#define vprintstr(a)   printstr(a)
#define vprinthexln(a) printhexln(a)
#define vprintstrln(a) printstrln(a)
#define vprintintln(a) printintln(a)
#else
#define vprintstr(a)
#define vprinthexln(a)
#define vprintstrln(a)
#define vprintintln(a)
#endif

#define vprint(a) { vprintstrln(a); return 1;}
#define vprinti(a) { vprintintln(a); return 1; }

#ifdef __XC__
#ifdef USE_STREAMING_CHANNELS
#define streaming_chanend streaming chanend
#else
#define streaming_chanend chanend
#endif
#else
#define streaming_chanend unsigned
#endif

#endif /*GLOBAL_H_*/

/**
 * Module:  module_ipod_support
 * Version: 1v0
 * Build:   7097304a351e95b4756e7a7125bb030ce017b929
 * File:    fifoRead.c
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

// Use c routines to bypass thread disjointedness rules

#include "pwmDefines.h"
#include <print.h>

// samples are written to from one thread, and read from another (this one)
int fifoRead (int fifoBasePtr, int fifoPtrPtr, int fifoCheckBasePtr, int fifoCheckVal) {
  int *fifoBase;
  int *fifoPtr;
  int readData;
  int *fifoCheckBase;
  int readCheckData;

  fifoBase = (int *)(fifoBasePtr);
  fifoPtr  = (int *)(fifoPtrPtr);
  fifoCheckBase = (int *)(fifoCheckBasePtr);

  // Check the sample number
  readCheckData = fifoCheckBase[*fifoPtr];
  #if PRINT == PWM_FLOW_CHECK
    if (readCheckData != fifoCheckVal)
    {
      printstrln("PWM Fifo Checking failure");
      printstr("Expected: ");
      printintln(fifoCheckVal);
      printstr("Actual:   ");
      printintln(readCheckData);
      while(1);
    }
  #endif

  // now perform read
  readData = fifoBase[*fifoPtr];
  (*fifoPtr)++;
  (*fifoPtr) = (*fifoPtr) & PWM_FIFO_MASK;
  return (readData);

}

// Read the sample rate attributes (written by another thread).
extern int attribute [NUM_ATTR_ELEMS];

int attr (int element)
{
  return (attribute [ element]);
}


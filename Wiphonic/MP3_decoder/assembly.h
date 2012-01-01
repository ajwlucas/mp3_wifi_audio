/**************************************************************************************
 * Modifications: Andrew Lucas
 * al7927@bristol.ac.uk
 * University of Bristol
 * 03/2010
 * 
 *************************************************************************************/

/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0/RPSL 1.0 
 *  
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved. 
 *      
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Public Source License 
 * Version 1.0 (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the RealNetworks Community Source License Version 1.0 
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl, 
 * in which case the RCSL will apply. You may also obtain the license terms 
 * directly from RealNetworks.  You may not use this file except in 
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks 
 * applicable to this file, the RCSL.  Please see the applicable RPSL or 
 * RCSL for the rights, obligations and limitations governing use of the 
 * contents of the file.  
 *  
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 *  
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 *    http://www.helixcommunity.org/content/tck 
 * 
 * Contributor(s): 
 *  
 * ***** END LICENSE BLOCK ***** */ 

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * assembly.h - assembly language functions and prototypes for supported platforms
 *
 * - inline rountines with access to 64-bit multiply results 
 * - x86 (_WIN32) and ARM (ARM_ADS, _WIN32_WCE) versions included
 * - some inline functions are mix of asm and C for speed
 * - some functions are in native asm files, so only the prototype is given here
 *
 * MULSHIFT32(x, y)    signed multiply of two 32-bit integers (x and y), returns top 32 bits of 64-bit result
 * FASTABS(x)          branchless absolute value of signed integer x
 * CLZ(x)              count leading zeros in x
 * MADD64(sum, x, y)   (Windows only) sum [64-bit] += x [32-bit] * y [32-bit]
 * SHL64(sum, x, y)    (Windows only) 64-bit left shift using __int64
 * SAR64(sum, x, y)    (Windows only) 64-bit right shift using __int64
 */

#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

// Signed multiply of two 32-bit integers (x and y), returns top 32 bits of 64-bit result
__inline static int MULSHIFT32(int x, int y)
{
	int msb = 0;
	int lsb = 0;
	  __asm__ __volatile__( "maccs %0, %1, %2, %3"
	                        : "+r" (msb), "+r" (lsb)
	                        : "r" (x), "r" (y) );
	return( msb );
}

// Absolute of signed integer
__inline static int FASTABS(int x)
{
	int sign;

	sign = x >> (sizeof(int) * 8 - 1);
	x ^= sign;
	x -= sign;

	return x;
}

// Count leading zeros
__inline static int CLZ(int x)
{
	// XMOS has an instruction to do this
    int tmp;
    __asm__ __volatile__( "clz %0,%1"
                          : "=r" (tmp)
                          : "r" (x) );
    return tmp;
	
	/*
	int numZeros;

	if (!x)
		return (sizeof(int) * 8);

	numZeros = 0;
	while (!(x & 0x80000000)) {
		numZeros++;
		x <<= 1;
	} 

	return numZeros; */
}

// 64-bit multiply accumulate
__inline static long long int MADD64(long long int sum, int x, int y)
{
	// MACCS
	// Apparently this C targets a maccs instruction
	// Xlinkers says it generates two unneccessary instruction
	/* sum += (long long int)x * (long long int)y;
	return sum;
	*/
	
	// This is a better assembly routine that should generate a single
	// MACCS instruction
	
	unsigned sum_lo = (unsigned)sum;
	int sum_hi = (int)(sum >> 32);
	unsigned result_lo;
	int result_hi;
	asm("maccs %1, %0, %4, %5" :
	      "=r"(result_lo), "=r"(result_hi) :
	      "0"(sum_lo), "1"(sum_hi), "r"(x), "r"(y));
	
	return result_lo | ((long long)result_hi << 32);
}

// A 64 bit right shift 
__inline static long long int SAR64(long long int x, int n)
{
	// SHL
	// ASHR
	// SHR
	  unsigned int xLo = (unsigned int) x;
	  int xHi = (int) (x >> 32);
	  int nComp = 32-n;
	  int tmp;
	  // Shortcut: n is always < 32. 
	  __asm__ __volatile__( "shl %2, %0, %3\n\t"  		// tmp <- xHi<<(32-n)
	                        "ashr %0, %0, %4\n\t"  		// xHi <- xHi>>n
	                        "shr %1, %1, %4\n\t"  		// xLo <- xLo>>n
	                        "or  %1, %1, %2\n\t"      	// xLo <= xLo || tmp
	                        : "+&r" (xHi), "+r" (xLo), "=&r" (tmp)
	                        : "r" (nComp), "r" (n) );
	  x = xLo | ((signed long long int)xHi << 32);
	  return( x );
}

#endif /* _ASSEMBLY_H */

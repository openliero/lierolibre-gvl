

/* @(#)w_pow.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* 
 * wrapper fd_pow(x,y) return x**y
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_pow(double x, double y)	/* wrapper fd_pow */
#else
	double fd_pow(x,y)			/* wrapper fd_pow */
	double x,y;
#endif
{
#ifdef FD_IEEE_LIBM
	return  __ieee754_pow(x,y);
#else
	double z;
	z=__ieee754_pow(x,y);
	if(_LIB_VERSION == _IEEE_|| fd_isnan(y)) return z;
	if(fd_isnan(x)) {
	    if(y==0.0) 
	        return __kernel_standard(x,y,42); /* fd_pow(NaN,0.0) */
	    else 
		return z;
	}
	if(x==0.0){ 
	    if(y==0.0)
	        return __kernel_standard(x,y,20); /* fd_pow(0.0,0.0) */
	    if(fd_finite(y)&&y<0.0)
	        return __kernel_standard(x,y,23); /* fd_pow(0.0,negative) */
	    return z;
	}
	if(!fd_finite(z)) {
	    if(fd_finite(x)&&fd_finite(y)) {
	        if(fd_isnan(z))
	            return __kernel_standard(x,y,24); /* fd_pow neg**non-int */
	        else 
	            return __kernel_standard(x,y,21); /* fd_pow overflow */
	    }
	} 
	if(z==0.0&&fd_finite(x)&&fd_finite(y))
	    return __kernel_standard(x,y,22); /* fd_pow underflow */
	return z;
#endif
}


/* @(#)w_scalb.c 1.3 95/01/18 */
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
 * wrapper fd_scalb(double x, double fn) is provide for
 * passing various standard test suite. One 
 * should use fd_scalbn() instead.
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#include <errno.h>

#ifdef __STDC__
#ifdef _SCALB_INT
	double fd_scalb(double x, int fn)		/* wrapper fd_scalb */
#else
	double fd_scalb(double x, double fn)	/* wrapper fd_scalb */
#endif
#else
	double fd_scalb(x,fn)			/* wrapper fd_scalb */
#ifdef _SCALB_INT
	double x; int fn;
#else
	double x,fn;
#endif
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_scalb(x,fn);
#else
	double z;
	z = __ieee754_scalb(x,fn);
	if(_LIB_VERSION == _IEEE_) return z;
	if(!(fd_finite(z)||fd_isnan(z))&&fd_finite(x)) {
	    return __kernel_standard(x,(double)fn,32); /* fd_scalb overflow */
	}
	if(z==0.0&&z!=x) {
	    return __kernel_standard(x,(double)fn,33); /* fd_scalb underflow */
	} 
#ifndef _SCALB_INT
	if(!fd_finite(fn)) errno = ERANGE;
#endif
	return z;
#endif 
}

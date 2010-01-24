
/* @(#)w_sinh.c 1.3 95/01/18 */
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
 * wrapper fd_sinh(x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_sinh(double x)		/* wrapper fd_sinh */
#else
	double fd_sinh(x)			/* wrapper fd_sinh */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_sinh(x);
#else
	double z; 
	z = __ieee754_sinh(x);
	if(_LIB_VERSION == _IEEE_) return z;
	if(!fd_finite(z)&&fd_finite(x)) {
	    return __kernel_standard(x,x,25); /* fd_sinh overflow */
	} else
	    return z;
#endif
}

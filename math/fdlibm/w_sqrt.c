
/* @(#)w_sqrt.c 1.3 95/01/18 */
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
 * wrapper fd_sqrt(x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_sqrt(double x)		/* wrapper fd_sqrt */
#else
	double fd_sqrt(x)			/* wrapper fd_sqrt */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_sqrt(x);
#else
	double z;
	z = __ieee754_sqrt(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x)) return z;
	if(x<0.0) {
	    return __kernel_standard(x,x,26); /* fd_sqrt(negative) */
	} else
	    return z;
#endif
}


/* @(#)w_acosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/* 
 * wrapper fd_acosh(x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_acosh(double x)		/* wrapper fd_acosh */
#else
	double fd_acosh(x)			/* wrapper fd_acosh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_acosh(x);
#else
	double z;
	z = __ieee754_acosh(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x)) return z;
	if(x<1.0) {
	        return __kernel_standard(x,x,29); /* fd_acosh(x<1) */
	} else
	    return z;
#endif
}

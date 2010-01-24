
/* @(#)w_exp.c 1.3 95/01/18 */
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
 * wrapper fd_exp(x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double
#else
static double
#endif
o_threshold=  7.09782712893383973096e+02,  /* 0x40862E42, 0xFEFA39EF */
u_threshold= -7.45133219101941108420e+02;  /* 0xc0874910, 0xD52D3051 */

#ifdef __STDC__
	double fd_exp(double x)		/* wrapper fd_exp */
#else
	double fd_exp(x)			/* wrapper fd_exp */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_exp(x);
#else
	double z;
	z = __ieee754_exp(x);
	if(_LIB_VERSION == _IEEE_) return z;
	if(fd_finite(x)) {
	    if(x>o_threshold)
	        return __kernel_standard(x,x,6); /* fd_exp overflow */
	    else if(x<u_threshold)
	        return __kernel_standard(x,x,7); /* fd_exp underflow */
	} 
	return z;
#endif
}

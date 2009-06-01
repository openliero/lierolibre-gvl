
/* @(#)w_log.c 1.3 95/01/18 */
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
 * wrapper fd_log(x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_log(double x)		/* wrapper fd_log */
#else
	double fd_log(x)			/* wrapper fd_log */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_log(x);
#else
	double z;
	z = __ieee754_log(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x) || x > 0.0) return z;
	if(x==0.0)
	    return __kernel_standard(x,x,16); /* fd_log(0) */
	else 
	    return __kernel_standard(x,x,17); /* fd_log(x<0) */
#endif
}

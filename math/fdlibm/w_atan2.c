
/* @(#)w_atan2.c 1.3 95/01/18 */
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
 * wrapper fd_atan2(y,x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_atan2(double y, double x)	/* wrapper fd_atan2 */
#else
	double fd_atan2(y,x)			/* wrapper fd_atan2 */
	double y,x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_atan2(y,x);
#else
	double z;
	z = __ieee754_atan2(y,x);
	if(_LIB_VERSION == _IEEE_||fd_isnan(x)||fd_isnan(y)) return z;
	if(x==0.0&&y==0.0) {
	        return __kernel_standard(y,x,3); /* fd_atan2(+-0,+-0) */
	} else
	    return z;
#endif
}

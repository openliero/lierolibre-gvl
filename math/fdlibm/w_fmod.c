
/* @(#)w_fmod.c 1.3 95/01/18 */
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
 * wrapper fd_fmod(x,y)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_fmod(double x, double y)	/* wrapper fd_fmod */
#else
	double fd_fmod(x,y)		/* wrapper fd_fmod */
	double x,y;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_fmod(x,y);
#else
	double z;
	z = __ieee754_fmod(x,y);
	if(_LIB_VERSION == _IEEE_ ||fd_isnan(y)||fd_isnan(x)) return z;
	if(y==0.0) {
	        return __kernel_standard(x,y,27); /* fd_fmod(x,0) */
	} else
	    return z;
#endif
}

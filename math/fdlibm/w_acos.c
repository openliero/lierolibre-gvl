
/* @(#)w_acos.c 1.3 95/01/18 */
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
 * wrap_acos(x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"


#ifdef __STDC__
	double fd_acos(double x)		/* wrapper fd_acos */
#else
	double fd_acos(x)			/* wrapper fd_acos */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_acos(x);
#else
	double z;
	z = __ieee754_acos(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x)) return z;
	if(fd_fabs(x)>1.0) {
	        return __kernel_standard(x,x,1); /* fd_acos(|x|>1) */
	} else
	    return z;
#endif
}

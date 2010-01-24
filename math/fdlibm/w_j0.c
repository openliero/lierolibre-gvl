
/* @(#)w_j0.c 1.3 95/01/18 */
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
 * wrapper fd_j0(double x), fd_y0(double x)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_j0(double x)		/* wrapper fd_j0 */
#else
	double fd_j0(x)			/* wrapper fd_j0 */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_j0(x);
#else
	double z = __ieee754_j0(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x)) return z;
	if(fd_fabs(x)>FD_X_TLOSS) {
	        return __kernel_standard(x,x,34); /* fd_j0(|x|>FD_X_TLOSS) */
	} else
	    return z;
#endif
}

#ifdef __STDC__
	double fd_y0(double x)		/* wrapper fd_y0 */
#else
	double fd_y0(x)			/* wrapper fd_y0 */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_y0(x);
#else
	double z;
	z = __ieee754_y0(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x) ) return z;
        if(x <= 0.0){
                if(x==0.0)
                    /* d= -one/(x-x); */
                    return __kernel_standard(x,x,8);
                else
                    /* d = zero/(x-x); */
                    return __kernel_standard(x,x,9);
        }
	if(x>FD_X_TLOSS) {
	        return __kernel_standard(x,x,35); /* fd_y0(x>FD_X_TLOSS) */
	} else
	    return z;
#endif
}

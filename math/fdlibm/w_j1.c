
/* @(#)w_j1.c 1.3 95/01/18 */
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
 * wrapper of fd_j1,fd_y1 
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_j1(double x)		/* wrapper fd_j1 */
#else
	double fd_j1(x)			/* wrapper fd_j1 */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_j1(x);
#else
	double z;
	z = __ieee754_j1(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x) ) return z;
	if(fd_fabs(x)>FD_X_TLOSS) {
	        return __kernel_standard(x,x,36); /* fd_j1(|x|>FD_X_TLOSS) */
	} else
	    return z;
#endif
}

#ifdef __STDC__
	double fd_y1(double x)		/* wrapper fd_y1 */
#else
	double fd_y1(x)			/* wrapper fd_y1 */
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_y1(x);
#else
	double z;
	z = __ieee754_y1(x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x) ) return z;
        if(x <= 0.0){
                if(x==0.0)
                    /* d= -one/(x-x); */
                    return __kernel_standard(x,x,10);
                else
                    /* d = zero/(x-x); */
                    return __kernel_standard(x,x,11);
        }
	if(x>FD_X_TLOSS) {
	        return __kernel_standard(x,x,37); /* fd_y1(x>FD_X_TLOSS) */
	} else
	    return z;
#endif
}

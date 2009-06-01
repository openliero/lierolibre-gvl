
/* @(#)w_lgamma_r.c 1.3 95/01/18 */
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
 * wrapper double fd_lgamma_r(double x, int *signgamp)
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_lgamma_r(double x, int *signgamp) /* wrapper fd_lgamma_r */
#else
	double fd_lgamma_r(x,signgamp)              /* wrapper fd_lgamma_r */
        double x; int *signgamp;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_lgamma_r(x,signgamp);
#else
        double y;
        y = __ieee754_lgamma_r(x,signgamp);
        if(_LIB_VERSION == _IEEE_) return y;
        if(!fd_finite(y)&&fd_finite(x)) {
            if(fd_floor(x)==x&&x<=0.0)
                return __kernel_standard(x,x,15); /* fd_lgamma pole */
            else
                return __kernel_standard(x,x,14); /* fd_lgamma overflow */
        } else
            return y;
#endif
}             


/* @(#)w_lgamma.c 1.3 95/01/18 */
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

/* double fd_lgamma(double x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_lgamma_r
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

extern int fd_signgam;

#ifdef __STDC__
	double fd_lgamma(double x)
#else
	double fd_lgamma(x)
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_lgamma_r(x,&fd_signgam);
#else
        double y;
        y = __ieee754_lgamma_r(x,&fd_signgam);
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

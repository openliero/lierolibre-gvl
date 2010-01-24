
/* @(#)w_gamma.c 1.3 95/01/18 */
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

/* double fd_gamma(double x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call fd_gamma_r
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

extern int fd_signgam;

#ifdef __STDC__
	double fd_gamma(double x)
#else
	double fd_gamma(x)
	double x;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_gamma_r(x,&fd_signgam);
#else
        double y;
        y = __ieee754_gamma_r(x,&fd_signgam);
        if(_LIB_VERSION == _IEEE_) return y;
        if(!fd_finite(y)&&fd_finite(x)) {
            if(fd_floor(x)==x&&x<=0.0)
                return __kernel_standard(x,x,41); /* fd_gamma pole */
            else
                return __kernel_standard(x,x,40); /* fd_gamma overflow */
        } else
            return y;
#endif
}             

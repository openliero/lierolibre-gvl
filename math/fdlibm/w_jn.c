
/* @(#)w_jn.c 1.3 95/01/18 */
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
 * wrapper fd_jn(int n, double x), fd_yn(int n, double x)
 * floating point Bessel's function of the 1st and 2nd kind
 * of order n
 *          
 * Special cases:
 *	fd_y0(0)=fd_y1(0)=fd_yn(n,0) = -inf with division by zero signal;
 *	fd_y0(-ve)=fd_y1(-ve)=fd_yn(n,-ve) are NaN with invalid signal.
 * Note 2. About fd_jn(n,x), fd_yn(n,x)
 *	For n=0, fd_j0(x) is called,
 *	for n=1, fd_j1(x) is called,
 *	for n<x, forward recursion us used starting
 *	from values of fd_j0(x) and fd_j1(x).
 *	for n>x, a continued fraction approximation to
 *	j(n,x)/j(n-1,x) is evaluated and then backward
 *	recursion is used starting from a supposed value
 *	for j(n,x). The resulting value of j(0,x) is
 *	compared with the actual value to correct the
 *	supposed value of j(n,x).
 *
 *	fd_yn(n,x) is similar in all respects, except
 *	that forward recursion is used for all
 *	values of n>1.
 *	
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
	double fd_jn(int n, double x)	/* wrapper fd_jn */
#else
	double fd_jn(n,x)			/* wrapper fd_jn */
	double x; int n;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_jn(n,x);
#else
	double z;
	z = __ieee754_jn(n,x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x) ) return z;
	if(fd_fabs(x)>FD_X_TLOSS) {
	    return __kernel_standard((double)n,x,38); /* fd_jn(|x|>FD_X_TLOSS,n) */
	} else
	    return z;
#endif
}

#ifdef __STDC__
	double fd_yn(int n, double x)	/* wrapper fd_yn */
#else
	double fd_yn(n,x)			/* wrapper fd_yn */
	double x; int n;
#endif
{
#ifdef FD_IEEE_LIBM
	return __ieee754_yn(n,x);
#else
	double z;
	z = __ieee754_yn(n,x);
	if(_LIB_VERSION == _IEEE_ || fd_isnan(x) ) return z;
        if(x <= 0.0){
                if(x==0.0)
                    /* d= -one/(x-x); */
                    return __kernel_standard((double)n,x,12);
                else
                    /* d = zero/(x-x); */
                    return __kernel_standard((double)n,x,13);
        }
	if(x>FD_X_TLOSS) {
	    return __kernel_standard((double)n,x,39); /* fd_yn(x>FD_X_TLOSS,n) */
	} else
	    return z;
#endif
}

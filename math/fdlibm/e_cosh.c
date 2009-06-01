
/* @(#)e_cosh.c 1.3 95/01/18 */
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

/* __ieee754_cosh(x)
 * Method :
 * mathematically fd_cosh(x) if defined to be (fd_exp(x)+fd_exp(-x))/2
 *	1. Replace x by |x| (fd_cosh(x) = fd_cosh(-x)).
 *	2.
 *		                                        [ fd_exp(x) - 1 ]^2
 *	    0        <= x <= ln2/2  :  fd_cosh(x) := 1 + -------------------
 *			       			           2*fd_exp(x)
 *
 *		                                  fd_exp(x) +  1/fd_exp(x)
 *	    ln2/2    <= x <= 22     :  fd_cosh(x) := -------------------
 *			       			          2
 *	    22       <= x <= lnovft :  fd_cosh(x) := fd_exp(x)/2
 *	    lnovft   <= x <= ln2ovft:  fd_cosh(x) := fd_exp(x/2)/2 * fd_exp(x/2)
 *	    ln2ovft  <  x	    :  fd_cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	fd_cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only fd_cosh(0)=1 is exact for fd_finite x.
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double one = 1.0, half=0.5, huge = 1.0e300;
#else
static double one = 1.0, half=0.5, huge = 1.0e300;
#endif

#ifdef __STDC__
	double __ieee754_cosh(double x)
#else
	double __ieee754_cosh(x)
	double x;
#endif
{
	double t,w;
	int ix;
	unsigned lx;

    /* High word of |x|. */
	ix = FD_HI(x);
	ix &= 0x7fffffff;

    /* x is INF or NaN */
	if(ix>=0x7ff00000) return gM(x,x);

    /* |x| in [0,0.5*ln2], return 1+fd_expm1(|x|)^2/(2*fd_exp(|x|)) */
	if(ix<0x3fd62e43) {
	    t = fd_expm1(fd_fabs(x));
	    w = gA(one,t);
	    if (ix<0x3c800000) return w;	/* fd_cosh(tiny) = 1 */
	    return gA(one, gD(gM(t,t), gA(w,w)));
	}

    /* |x| in [0.5*ln2,22], return (fd_exp(|x|)+1/fd_exp(|x|)/2; */
	if (ix < 0x40360000) {
		t = __ieee754_exp(fd_fabs(x));
		return gA(gM(half,t), gD(half,t));
	}

    /* |x| in [22, fd_log(maxdouble)] return half*fd_exp(|x|) */
	if (ix < 0x40862E42)  return gM(half,__ieee754_exp(fd_fabs(x)));

    /* |x| in [fd_log(maxdouble), overflowthresold] */
	lx = *( (((*(unsigned*)&one)>>29)) + (unsigned*)&x);
	if (ix<0x408633CE ||
	      (ix==0x408633ce)&&(lx<=(unsigned)0x8fb9f87d)) {
	    w = __ieee754_exp(gM(half, fd_fabs(x)));
	    t = gM(half,w);
	    return gM(t,w);
	}

    /* |x| > overflowthresold, fd_cosh(x) overflow */
	return gM(huge,huge);
}

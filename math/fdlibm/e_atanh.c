
/* @(#)e_atanh.c 1.3 95/01/18 */
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

/* __ieee754_atanh(x)
 * Method :
 *    1.Reduced x to positive by fd_atanh(-x) = -fd_atanh(x)
 *    2.For x>=0.5
 *                  1              2x                          x
 *	fd_atanh(x) = --- * fd_log(1 + -------) = 0.5 * fd_log1p(2 * --------)
 *                  2             1 - x                      1 - x
 *
 * 	For x<0.5
 *	fd_atanh(x) = 0.5*fd_log1p(2x+2x*x/(1-x))
 *
 * Special cases:
 *	fd_atanh(x) is NaN if |x| > 1 with signal;
 *	fd_atanh(NaN) is that NaN with no signal;
 *	fd_atanh(+-1) is +-INF with signal.
 *
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double one = 1.0, huge = 1e300;
#else
static double one = 1.0, huge = 1e300;
#endif

static double zero = 0.0;

#ifdef __STDC__
	double __ieee754_atanh(double x)
#else
	double __ieee754_atanh(x)
	double x;
#endif
{
	double t;
	int hx,ix;
	unsigned lx;
	hx = FD_HI(x);		/* high word */
	lx = FD_LO(x);		/* low word */
	ix = hx&0x7fffffff;
	if ((ix|((lx|(-lx))>>31))>0x3ff00000) /* |x|>1 */
	    return gD(gS(x,x), gS(x,x));
	if(ix==0x3ff00000)
	    return gD(x,zero);
	if(ix<0x3e300000 && gA(huge,x)>zero) return x;	/* x<2**-28 */
	FD_HI(x) = ix;		/* x <- |x| */
	if(ix<0x3fe00000) {		/* x < 0.5 */
	    t = gA(x,x);
	    t = gM(0.5, fd_log1p(gA(t, gD(gM(t,x), gS(one,x)))));
	} else
	    t = gM(0.5, fd_log1p(gD(gA(x,x), gS(one,x))));
	if(hx>=0) return t; else return -t;
}

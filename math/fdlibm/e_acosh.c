
/* @(#)e_acosh.c 1.3 95/01/18 */
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

/* __ieee754_acosh(x)
 * Method :
 *	Based on
 *		fd_acosh(x) = fd_log [ x + fd_sqrt(x*x-1) ]
 *	we have
 *		fd_acosh(x) := fd_log(x)+ln2,	if x is large; else
 *		fd_acosh(x) := fd_log(2x-1/(fd_sqrt(x*x-1)+x)) if x>2; else
 *		fd_acosh(x) := fd_log1p(t+fd_sqrt(2.0*t+t*t)); where t=x-1.
 *
 * Special cases:
 *	fd_acosh(x) is NaN with signal if x<1.
 *	fd_acosh(NaN) is NaN without signal.
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double
#else
static double
#endif
one	= 1.0,
ln2	= 6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */

#ifdef __STDC__
	double __ieee754_acosh(double x)
#else
	double __ieee754_acosh(x)
	double x;
#endif
{
	double t;
	int hx;
	hx = FD_HI(x);
	if(hx<0x3ff00000) {		/* x < 1 */
	    return gD(gS(x,x), gS(x,x));
	} else if(hx >=0x41b00000) {	/* x > 2**28 */
	    if(hx >=0x7ff00000) {	/* x is inf of NaN */
	        return gA(x,x);
	    } else
		return gA(__ieee754_log(x), ln2);	/* fd_acosh(huge)=fd_log(2x) */
	} else if(((hx-0x3ff00000)|FD_LO(x))==0) {
	    return 0.0;			/* fd_acosh(1) = 0 */
	} else if (hx > 0x40000000) {	/* 2**28 > x > 2 */
	    t = gM(x,x);
	    return __ieee754_log(gM(2.0,x) - gD(one,gA(x,gSqrt(gS(t,one)))));
	} else {			/* 1<x<2 */
	    t = gS(x,one);
	    return fd_log1p(gA(t, gSqrt(gA(gM(2.0,t), gM(t,t)))));
	}
}

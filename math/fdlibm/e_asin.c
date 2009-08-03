
/* @(#)e_asin.c 1.3 95/01/18 */
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

/* __ieee754_asin(x)
 * Method :
 *	Since  fd_asin(x) = x + x^3/6 + x^5*3/40 + x^7*15/336 + ...
 *	we approximate fd_asin(x) on [0,0.5] by
 *		fd_asin(x) = x + x*x^2*R(x^2)
 *	where
 *		R(x^2) is a rational approximation of (fd_asin(x)-x)/x^3
 *	and its remez error is bounded by
 *		|(fd_asin(x)-x)/x^3 - R(x^2)| < 2^(-58.75)
 *
 *	For x in [0.5,1]
 *		fd_asin(x) = pi/2-2*fd_asin(fd_sqrt((1-x)/2))
 *	Let y = (1-x), z = y/2, s := fd_sqrt(z), and pio2_hi+pio2_lo=pi/2;
 *	then for x>0.98
 *		fd_asin(x) = pi/2 - 2*(s+s*z*R(z))
 *			= pio2_hi - (2*(s+s*z*R(z)) - pio2_lo)
 *	For x<=0.98, let pio4_hi = pio2_hi/2, then
 *		f = hi part of s;
 *		c = fd_sqrt(z) - f = (z-f*f)/(s+f) 	...f+c=fd_sqrt(z)
 *	and
 *		fd_asin(x) = pi/2 - 2*(s+s*z*R(z))
 *			= pio4_hi+(pio4-2s)-(2s*z*R(z)-pio2_lo)
 *			= pio4_hi+(pio4-2f)-(2s*z*R(z)-(pio2_lo+2c))
 *
 * Special cases:
 *	if x is NaN, return x itself;
 *	if |x|>1, return NaN with invalid signal.
 *
 */


#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double
#else
static double
#endif
one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
huge =  1.000e+300,
pio2_hi =  1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
pio2_lo =  6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
pio4_hi =  7.85398163397448278999e-01, /* 0x3FE921FB, 0x54442D18 */
	/* coefficient for R(x^2) */
pS0 =  1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
pS1 = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
pS2 =  2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
pS3 = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
pS4 =  7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
pS5 =  3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
qS1 = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
qS2 =  2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
qS3 = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

#ifdef __STDC__
	double __ieee754_asin(double x)
#else
	double __ieee754_asin(x)
	double x;
#endif
{
	double t,w,p,q,c,r,s;
	int hx,ix;
	hx = FD_HI(x);
	ix = hx&0x7fffffff;
	if(ix>= 0x3ff00000) {		/* |x|>= 1 */
		if(((ix-0x3ff00000)|FD_LO(x))==0)
			/* fd_asin(1)=+-pi/2 with inexact */
		return gA(gM(x,pio2_hi), gM(x,pio2_lo));
		return gD(gS(x,x),gS(x,x));		/* fd_asin(|x|>1) is NaN */
	} else if (ix<0x3fe00000) {	/* |x|<0.5 */
		if(ix<0x3e400000) {		/* if |x| < 2**-27 */
		if(gA(huge,x) > one) return x;/* return x with inexact if x!=0*/
		} else
		t = gM(x,x);
		p = gM(t,gA(pS0, gM(t,gA(pS1, gM(t,gA(pS2, gM(t,gA(pS3, gM(t,gA(pS4, gM(t,pS5)))))))))));
		q = gA(one, gM(t,gA(qS1, gM(t,gA(qS2, gM(t,gA(qS3, gM(t,qS4))))))));
		w = gD(p, q);
		return gA(x, gM(x,w));
	}
	/* 1> |x|>= 0.5 */
	w = gS(one,fd_fabs(x));
	t = gM(w,0.5);
	p = gM(t,gA(pS0, gM(t,gA(pS1, gM(t,gA(pS2, gM(t,gA(pS3, gM(t,gA(pS4, gM(t,pS5)))))))))));
	q = gA(one, gM(t,gA(qS1, gM(t,gA(qS2, gM(t,gA(qS3, gM(t,qS4))))))));
	s = gSqrt(t);
	if(ix>=0x3FEF3333) { 	/* if |x| > 0.975 */
		w = gD(p,q);
		t = gS(pio2_hi, gS(gM(2.0, gA(s, gM(s,w))), pio2_lo));
	} else {
		w  = s;
		FD_LO(w) = 0;
		c  = gD(gS(t, gM(w,w)), gA(s,w));
		r  = gD(p,q);
		p  = gS(gM(gM(2.0,s),r), gS(pio2_lo,gM(2.0,c)));
		q  = gS(pio4_hi, gM(2.0,w));
		t  = gS(pio4_hi, gS(p,q));
	}
	if(hx>0) return t; else return -t;
}

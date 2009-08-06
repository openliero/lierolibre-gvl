
/* @(#)e_acos.c 1.3 95/01/18 */
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

/* __ieee754_acos(x)
 * Method :
 *	fd_acos(x)  = pi/2 - fd_asin(x)
 *	fd_acos(-x) = pi/2 + fd_asin(x)
 * For |x|<=0.5
 *	fd_acos(x) = pi/2 - (x + x*x^2*R(x^2))	(see fd_asin.c)
 * For x>0.5
 * 	fd_acos(x) = pi/2 - (pi/2 - 2asin(fd_sqrt((1-x)/2)))
 *		= 2asin(fd_sqrt((1-x)/2))
 *		= 2s + 2s*z*R(z) 	...z=(1-x)/2, s=fd_sqrt(z)
 *		= 2f + (2c + 2s*z*R(z))
 *     where f=hi part of s, and c = (z-f*f)/(s+f) is the correction term
 *     for f so that f+c ~ fd_sqrt(z).
 * For x<-0.5
 *	fd_acos(x) = pi - 2asin(fd_sqrt((1-|x|)/2))
 *		= pi - 0.5*(s+s*z*R(z)), where z=(1-|x|)/2,s=fd_sqrt(z)
 *
 * Special cases:
 *	if x is NaN, return x itself;
 *	if |x|>1, return NaN with invalid signal.
 *
 * Function needed: fd_sqrt
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double
#else
static double
#endif
one=  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
pi =  3.14159265358979311600e+00, /* 0x400921FB, 0x54442D18 */
pio2_hi =  1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
pio2_lo =  6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
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
	double __ieee754_acos(double x)
#else
	double __ieee754_acos(x)
	double x;
#endif
{
	double z,p,q,r,w,s,c,df;
	int hx,ix;
	hx = FD_HI(x);
	ix = hx&0x7fffffff;
	if(ix>=0x3ff00000) {	/* |x| >= 1 */
		if(((ix-0x3ff00000)|FD_LO(x))==0) {	/* |x|==1 */
		if(hx>0) return 0.0;		/* fd_acos(1) = 0  */
		else return gA(pi, gM(2.0, pio2_lo));	/* fd_acos(-1)= pi */
		}
		return gD(gS(x,x), gS(x,x));		/* fd_acos(|x|>1) is NaN */
	}
	if(ix<0x3fe00000) {	/* |x| < 0.5 */
		if(ix<=0x3c600000) return gA(pio2_hi, pio2_lo);/*if|x|<2**-57*/
		z = gM(x, x);
		p = gM(z,gA(pS0,gM(z,gA(pS1,gM(z,gA(pS2,gM(z,gA(pS3,gM(z,gA(pS4,gM(z,pS5)))))))))));
		q = gA(one,gM(z,gA(qS1,gM(z,gA(qS2,gM(z,gA(qS3,gM(z,qS4))))))));
		r = gD(p,q);
		return gS(pio2_hi, gS(x, gS(pio2_lo, gM(x,r))));
	} else  if (hx<0) {		/* x < -0.5 */
		z = gM(gA(one, x), 0.5);
		p = gM(z, gA(pS0,gM(z,gA(pS1,gM(z,gA(pS2,gM(z,gA(pS3,gM(z,gA(pS4,gM(z,pS5)))))))))));
		q = gA(one, gM(z,gA(qS1,gM(z,gA(qS2,gM(z,gA(qS3,gM(z,qS4))))))));
		s = gSqrt(z);
		r = gD(p, q);
		w = gS(gM(r, s), pio2_lo);
		return gS(pi, gM(2.0, gA(s,w)));
	} else {			/* x > 0.5 */
		z = gM(gS(one, x), 0.5);
		s = gSqrt(z);
		df = s;
		FD_LO(df) = 0;
		c  = gD(gS(z, gM(df,df)), gA(s,df));
		p = gM(z,gA(pS0,gM(z,gA(pS1,gM(z,gA(pS2,gM(z,gA(pS3,gM(z,gA(pS4,gM(z,pS5)))))))))));
		q = gA(one, gM(z,gA(qS1,gM(z,gA(qS2,gM(z,gA(qS3,gM(z,qS4))))))));
		r = gD(p, q);
		w = gA(gM(r, s), c);
		return gM(2.0, gA(df, w));
	}
}

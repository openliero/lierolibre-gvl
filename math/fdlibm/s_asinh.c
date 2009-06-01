
/* @(#)s_asinh.c 1.3 95/01/18 */
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

/* fd_asinh(x)
 * Method :
 *	Based on 
 *		fd_asinh(x) = sign(x) * fd_log [ |x| + fd_sqrt(x*x+1) ]
 *	we have
 *	fd_asinh(x) := x  if  1+x*x=1,
 *		 := sign(x)*(fd_log(x)+ln2)) for large |x|, else
 *		 := sign(x)*fd_log(2|x|+1/(|x|+fd_sqrt(x*x+1))) if|x|>2, else
 *		 := sign(x)*fd_log1p(|x| + x^2/(1 + fd_sqrt(1+x^2)))  
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double 
#else
static double 
#endif
one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
ln2 =  6.93147180559945286227e-01, /* 0x3FE62E42, 0xFEFA39EF */
huge=  1.00000000000000000000e+300; 

#ifdef __STDC__
	double fd_asinh(double x)
#else
	double fd_asinh(x)
	double x;
#endif
{	
	double t,w;
	int hx,ix;
	hx = FD_HI(x);
	ix = hx&0x7fffffff;
	if(ix>=0x7ff00000) return x+x;	/* x is inf or NaN */
	if(ix< 0x3e300000) {	/* |x|<2**-28 */
	    if(gA(huge,x) > one) return x;	/* return x inexact except 0 */
	} 
	if(ix>0x41b00000) {	/* |x| > 2**28 */
	    w = gA(__ieee754_log(fd_fabs(x)), ln2);
	} else if (ix>0x40000000) {	/* 2**28 > |x| > 2.0 */
	    t = fd_fabs(x);
	    w = __ieee754_log(gA(gM(2.0,t), gD(one,gA(gSqrt( gA(gM(x,x), one) ), t))));
	} else {		/* 2.0 > |x| > 2**-28 */
	    t = gM(x,x);
	    w = fd_log1p(gA(fd_fabs(x), gD(t,gA(one,gSqrt( gA(one,t) )))));
	}
	if(hx>0) return w; else return -w;
}

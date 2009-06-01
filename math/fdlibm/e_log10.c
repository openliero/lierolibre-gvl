
/* @(#)e_log10.c 1.3 95/01/18 */
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

/* __ieee754_log10(x)
 * Return the base 10 logarithm of x
 *
 * Method :
 *	Let log10_2hi = leading 40 bits of fd_log10(2) and
 *	    log10_2lo = fd_log10(2) - log10_2hi,
 *	    ivln10   = 1/fd_log(10) rounded.
 *	Then
 *		n = fd_ilogb(x),
 *		if(n<0)  n = n+1;
 *		x = fd_scalbn(x,-n);
 *		fd_log10(x) := n*log10_2hi + (n*log10_2lo + ivln10*fd_log(x))
 *
 * Note 1:
 *	To guarantee fd_log10(10**n)=n, where 10**n is normal, the rounding
 *	mode must set to Round-to-Nearest.
 * Note 2:
 *	[1/fd_log(10)] rounded to 53 bits has error  .198   ulps;
 *	fd_log10 is monotonic at all binary break points.
 *
 * Special cases:
 *	fd_log10(x) is NaN with signal if x < 0;
 *	fd_log10(+INF) is +INF with no signal; fd_log10(0) is -INF with signal;
 *	fd_log10(NaN) is that NaN with no signal;
 *	fd_log10(10**N) = N  for N=0,1,...,22.
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following constants.
 * The decimal values may be used, provided that the compiler will convert
 * from decimal to binary accurately enough to produce the hexadecimal values
 * shown.
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef __STDC__
static const double
#else
static double
#endif
two54      =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
ivln10     =  4.34294481903251816668e-01, /* 0x3FDBCB7B, 0x1526E50E */
log10_2hi  =  3.01029995663611771306e-01, /* 0x3FD34413, 0x509F6000 */
log10_2lo  =  3.69423907715893078616e-13; /* 0x3D59FEF3, 0x11F12B36 */

static double zero   =  0.0;

#ifdef __STDC__
	double __ieee754_log10(double x)
#else
	double __ieee754_log10(x)
	double x;
#endif
{
	double y,z;
	int i,k,hx;
	unsigned lx;

	hx = FD_HI(x);	/* high word of x */
	lx = FD_LO(x);	/* low word of x */

        k=0;
        if (hx < 0x00100000) {                  /* x < 2**-1022  */
            if (((hx&0x7fffffff)|lx)==0)
                return gD(-two54,zero);             /* fd_log(+-0)=-inf */
            if (hx<0) return gD(gS(x,x),zero);        /* fd_log(-#) = NaN */
            k -= 54; x = gM(x,two54); /* subnormal number, scale up x */
            hx = FD_HI(x);                /* high word of x */
        }
	if (hx >= 0x7ff00000) return gA(x,x);
	k += (hx>>20)-1023;
	i  = ((unsigned)k&0x80000000)>>31;
        hx = (hx&0x000fffff)|((0x3ff-i)<<20);
        y  = (double)(k+i);
        FD_HI(x) = hx;
	z  = gA(gM(y,log10_2lo), gM(ivln10,__ieee754_log(x)));
	return  gA(z, gM(y,log10_2hi));
}

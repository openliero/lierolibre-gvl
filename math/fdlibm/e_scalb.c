
/* @(#)e_scalb.c 1.3 95/01/18 */
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
 * __ieee754_scalb(x, fn) is provide for
 * passing various standard test suite. One 
 * should use fd_scalbn() instead.
 */

#include "fdlibm.h"
#include "fdlibm_intern.h"

#ifdef _SCALB_INT
#ifdef __STDC__
	double __ieee754_scalb(double x, int fn)
#else
	double __ieee754_scalb(x,fn)
	double x; int fn;
#endif
#else
#ifdef __STDC__
	double __ieee754_scalb(double x, double fn)
#else
	double __ieee754_scalb(x,fn)
	double x, fn;
#endif
#endif
{
#ifdef _SCALB_INT
	return fd_scalbn(x,fn);
#else
	if (fd_isnan(x)||fd_isnan(fn)) return gM(x,fn);
	if (!fd_finite(fn)) {
	    if(fn>0.0) return gM(x,fn);
	    else       return gD(x,(-fn));
	}
	if (fd_rint(fn)!=fn) return gD(gS(fn,fn), gS(fn,fn));
	if ( fn > 65000.0) return fd_scalbn(x, 65000);
	if (-fn > 65000.0) return fd_scalbn(x,-65000);
	return fd_scalbn(x,(int)fn);
#endif
}

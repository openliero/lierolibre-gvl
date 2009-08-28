#ifndef UUID_0DFC0705C06145FD593785907FBDDF54
#define UUID_0DFC0705C06145FD593785907FBDDF54

#include "../../support/platform.hpp"


/* @(#)fdlibm.h 1.5 95/01/18 */
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

#ifdef __cplusplus
extern "C" {
#endif

/*
inline double fd_r(double x)
{
   return x;
}*/

#define _IEEE_LIBM


#if GVL_LITTLE_ENDIAN
#define FD_HI(x) *(1+(int*)&x)
#define FD_LO(x) *(int*)&x
#define FD_HIp(x) *(1+(int*)x)
#define FD_LOp(x) *(int*)x
#else
#define FD_HI(x) *(int*)&x
#define FD_LO(x) *(1+(int*)&x)
#define FD_HIp(x) *(int*)x
#define FD_LOp(x) *(1+(int*)x)
#endif

#ifndef __STDC__
#define __STDC__ 1
#endif

#define	fd_P(p)	p

/*
 * ANSI/POSIX
 */

extern int signgam;

#define	FD_MAXFLOAT	((float)3.40282346638528860e+38)

enum fdversion {fdlibm_ieee = -1, fdlibm_svid, fdlibm_xopen, fdlibm_posix};

#define _LIB_VERSION_TYPE enum fdversion
#define _LIB_VERSION _fdlib_version

/* if global variable _LIB_VERSION is not desirable, one may
 * change the following to be a constant by:
 *	#define _LIB_VERSION_TYPE const enum version
 * In that case, after one initializes the value _LIB_VERSION (see
 * s_lib_version.c) during compile time, it cannot be modified
 * in the middle of a program
 */
extern  _LIB_VERSION_TYPE  _LIB_VERSION;

#define _IEEE_  fdlibm_ieee
#define _SVID_  fdlibm_svid
#define _XOPEN_ fdlibm_xopen
#define _POSIX_ fdlibm_posix

struct fd_exception {
	int type;
	char *name;
	double arg1;
	double arg2;
	double retval;
};

#define	FD_HUGE		FD_MAXFLOAT

/*
 * set FD_X_TLOSS = pi*2**52, which is possibly defined in <values.h>
 * (one may replace the following line by "#include <values.h>")
 */

#define FD_X_TLOSS		1.41484755040568800000e+16

#define	FD_DOMAIN		1
#define	FD_SING		2
#define	FD_OVERFLOW	3
#define	FD_UNDERFLOW	4
#define	FD_TLOSS		5
#define	FD_PLOSS		6

/*
 * ANSI/POSIX
 */
extern double fd_acos fd_P((double));
extern double fd_asin fd_P((double));
extern double fd_atan fd_P((double));
extern double fd_atan2 fd_P((double, double));
extern double fd_cos fd_P((double));
extern double fd_sin fd_P((double));
extern double fd_tan fd_P((double));

extern double fd_cosh fd_P((double));
extern double fd_sinh fd_P((double));
extern double fd_tanh fd_P((double));

extern double fd_exp fd_P((double));
extern double fd_frexp fd_P((double, int *));
extern double fd_ldexp fd_P((double, int));
extern double fd_log fd_P((double));
extern double fd_log10 fd_P((double));
extern double fd_modf fd_P((double, double *));

extern double fd_pow fd_P((double, double));
extern double fd_sqrt fd_P((double));

extern double fd_ceil fd_P((double));
extern double fd_fabs fd_P((double));
extern double fd_floor fd_P((double));
extern double fd_fmod fd_P((double, double));

extern double fd_erf fd_P((double));
extern double fd_erfc fd_P((double));
extern double fd_gamma fd_P((double));
extern double fd_hypot fd_P((double, double));
extern int fd_isnan fd_P((double));
extern int fd_finite fd_P((double));
extern double fd_j0 fd_P((double));
extern double fd_j1 fd_P((double));
extern double fd_jn fd_P((int, double));
extern double fd_lgamma fd_P((double));
extern double fd_y0 fd_P((double));
extern double fd_y1 fd_P((double));
extern double fd_yn fd_P((int, double));

extern double fd_acosh fd_P((double));
extern double fd_asinh fd_P((double));
extern double fd_atanh fd_P((double));
extern double fd_cbrt fd_P((double));
extern double fd_logb fd_P((double));
extern double fd_nextafter fd_P((double, double));
extern double fd_remainder fd_P((double, double));
#ifdef _SCALB_INT
extern double fd_scalb fd_P((double, int));
#else
extern double fd_scalb fd_P((double, double));
#endif

extern int fd_matherr fd_P((struct fd_exception *));

/*
 * IEEE Test Vector
 */
extern double fd_significand fd_P((double));

/*
 * Functions callable from C, intended to support IEEE arithmetic.
 */
extern double fd_copysign fd_P((double, double));
extern int fd_ilogb fd_P((double));
extern double fd_rint fd_P((double));
extern double fd_scalbn fd_P((double, int));

/*
 * BSD math library entry points
 */
extern double fd_expm1 fd_P((double));
extern double fd_log1p fd_P((double));

/*
 * Reentrant version of fd_gamma & fd_lgamma; passes signgam back by reference
 * as the second argument; user must allocate space for signgam.
 */
#ifdef _REENTRANT
extern double fd_gamma_r fd_P((double, int *));
extern double fd_lgamma_r fd_P((double, int *));
#endif	/* _REENTRANT */

/* ieee style elementary functions */
extern double __ieee754_sqrt fd_P((double));
extern double __ieee754_acos fd_P((double));
extern double __ieee754_acosh fd_P((double));
extern double __ieee754_log fd_P((double));
extern double __ieee754_atanh fd_P((double));
extern double __ieee754_asin fd_P((double));
extern double __ieee754_atan2 fd_P((double,double));
extern double __ieee754_exp fd_P((double));
extern double __ieee754_cosh fd_P((double));
extern double __ieee754_fmod fd_P((double,double));
extern double __ieee754_pow fd_P((double,double));
extern double __ieee754_lgamma_r fd_P((double,int *));
extern double __ieee754_gamma_r fd_P((double,int *));
extern double __ieee754_lgamma fd_P((double));
extern double __ieee754_gamma fd_P((double));
extern double __ieee754_log10 fd_P((double));
extern double __ieee754_sinh fd_P((double));
extern double __ieee754_hypot fd_P((double,double));
extern double __ieee754_j0 fd_P((double));
extern double __ieee754_j1 fd_P((double));
extern double __ieee754_y0 fd_P((double));
extern double __ieee754_y1 fd_P((double));
extern double __ieee754_jn fd_P((int,double));
extern double __ieee754_yn fd_P((int,double));
extern double __ieee754_remainder fd_P((double,double));
extern int    __ieee754_rem_pio2 fd_P((double,double*));
#ifdef _SCALB_INT
extern double __ieee754_scalb fd_P((double,int));
#else
extern double __ieee754_scalb fd_P((double,double));
#endif

/* fdlibm kernel function */
extern double __kernel_standard fd_P((double,double,int));
extern double __kernel_sin fd_P((double,double,int));
extern double __kernel_cos fd_P((double,double));
extern double __kernel_tan fd_P((double,double,int));
extern int    __kernel_rem_pio2 fd_P((double*,double*,int,int,int,const int*));

#ifdef __cplusplus
}
#endif

#endif // UUID_0DFC0705C06145FD593785907FBDDF54

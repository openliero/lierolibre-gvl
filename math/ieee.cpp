#include "ieee.hpp"


#if GVL_WIN32
#include <fpieee.h>
#include <excpt.h>
#endif

#include <float.h>
#include <stddef.h>
#include <stdio.h>

#if GVL_X87

// Exact 80-bit floating point little endian representation of 2^(16383 - 1023)
extern unsigned char const scaleup[10] = {0,0,0,0,0,0,0,128,255,123};

// Exact 80-bit floating point little endian representation of 1 / 2^(16383 - 1023)
extern unsigned char const scaledown[10] = {0,0,0,0,0,0,0,128,255,3};


#endif

void gvl_init_ieee()
{
#if GVL_MSVCPP
// Nothing needs to be done, TODO: we should however check that the x87 state is right
#else
    unsigned int const flags = _RC_NEAR | _PC_53 | _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT;
    _control87(flags, _MCW_EM | _MCW_PC | _MCW_RC);
#endif
}

int fpieee_handler( _FPIEEE_RECORD *pieee )
{
	pieee->Result.Value.Fp64Value = 0.0; // Flush to zero
	return EXCEPTION_CONTINUE_EXECUTION;
}

void gvl_flush_to_zero_context(void(*func)())
{
#if GVL_MSVCPP
	__try
	{
		unsigned int const flags = _EM_INVALID | /*_EM_DENORMAL |*/ _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT;
		unsigned int old;

		_controlfp_s(&old, flags, _MCW_EM);

		func();

		_controlfp_s(0, old, _MCW_EM);
	}
	__except(_fpieee_flt(
		GetExceptionCode(),
		GetExceptionInformation(),
		fpieee_handler))
	{
	}
#else
	fprintf(stderr, "gvl_flush_to_zero_context is unsupported");
	exit(1);
#endif
}

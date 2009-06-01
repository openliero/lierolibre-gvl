#ifndef UUID_9A3CA425DFA94C58CFBDEB99F70179F2
#define UUID_9A3CA425DFA94C58CFBDEB99F70179F2

#include <cmath>

#include "../support/platform.hpp"

namespace gvl
{

#if GVL_MSVCPP
inline long lrint(double x)
{
	long r;
	__asm
	{
		fld x
		fistp r
	}
		
	return r;
}

#elif GVL_GCC && GVL_ARCH == GVL_X86

// TODO: Not tested at all
inline long lrint(double x)
{
	long ret;
	__asm__ __volatile__ (
		"fldq %0;"
		"fistpl %1;" : "m" (x), "=m" (ret) : : "st") ;
	return ret;
}
#endif

inline long round_floor(double x)
{
	return lrint(x - 0.5);
}

inline long round_ceil(double x)
{
	return lrint(x + 0.5);
}

} // namespace gvl

#endif // UUID_9A3CA425DFA94C58CFBDEB99F70179F2

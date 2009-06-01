#include <gvl/tut/tut.hpp>

#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <gvl/math/fdlibm/fdlibm.h>
#include <cmath>

#include <gvl/math/ieee.hpp>
#include <emmintrin.h>

#pragma intrinsic(_mm_mul_sd, _mm_div_sd)

namespace tut
{

struct deterministic_float_data
{
	
};

typedef test_group<deterministic_float_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("deterministic floats");
} // namespace

namespace tut
{

bool equivalent(double x, double y)
{
	return (x == y) || (fd_isnan(x) && fd_isnan(y));
}


template<>
template<>
void object::test<1>()
{
#if 0
	gvl::tt800 rnd(1);
	

	for(int i = 0; i < 1000000000; ++i)
	{
		double x = 0.0;
		FD_LO(x) = rnd();
		FD_HI(x) = rnd();
		
		
		
		double rn = std::sqrt(x);
		double rfd = fd_sqrt(x);
		
		if(!equivalent(rn, rfd))
		{
			printf("std::sqrt(%f) != fd_sqrt(%f)\n", x, x);
		}
	}
#endif
}

inline double M(double x, double y)
{
	double r;
	_mm_store_sd(&r, _mm_mul_sd(_mm_load_sd(&x), _mm_load_sd(&y)));
	return r;
}

inline double D(double x, double y)
{
	double r;
	_mm_store_sd(&r, _mm_div_sd(_mm_load_sd(&x), _mm_load_sd(&y)));
	return r;
}

template<>
template<>
void object::test<2>()
{
	gvl::tt800 rnd(1);
		
	long trials = 0;
	long failures = 0;
	long incorrect_overflows = 0;
	
	unsigned char scaleup[10], scaledown[10];
	
	int correction_exponent = (32766 - 16383) - (2046 - 1023);
	
	*((unsigned short*)&scaleup[8]) = correction_exponent + 16383;
	*((unsigned long long*)&scaleup[0]) = 0x8000000000000000ull;
	*((unsigned short*)&scaledown[8]) = 16383 - correction_exponent;
	*((unsigned long long*)&scaledown[0]) = 0x8000000000000000ull;
	
	/*
	double scaledown = -correction_exponent;
	double scaleup = correction_exponent;
	*/
	
	double testd = 3.0;
	unsigned char test[10];
	
	for(int i = 0; i < 2000000000; ++i)
	{
		double x = 0.0, y = 0.0;
		FD_LO(x) = rnd();
		FD_HI(x) = rnd();
		FD_LO(y) = rnd();
		FD_HI(y) = rnd();
		
		double r1, r2;
		
#if 0
		__asm
		{
			fld TBYTE PTR scaleup
			fld TBYTE PTR scaledown
			fmul x // ST(0) = x*scaledown, ST(1) = scaleup
			fmul y // ST(0) = x*scaledown*y, ...
			fmul ST(0),ST(1) // ST(0) = x*scaledown*y*scaleup
			fstp r1
			fstp ST
			
			movsd xmm0, x
			mulsd xmm0, y
			movsd r2, xmm0
		}
#elif 0
		__asm
		{
			fld TBYTE PTR scaleup
			fld TBYTE PTR scaledown
			fmul x // ST(0) = x*scaledown, ST(1) = scaleup
			fdiv y // ST(0) = x*scaledown/y, ...
			fmul ST(0),ST(1) // ST(0) = x*scaledown/y*scaleup
			fstp r1
			fstp ST
			
			movsd xmm0, x
			divsd xmm0, y
			movsd r2, xmm0
		}
#elif 0
		__asm
		{
			fld x
			fadd y
			fstp r1
			
			/*
			movsd xmm0, x
			addsd xmm0, y
			movsd r2, xmm0*/
		}	
#endif

		r1 = D(M(x, x) + y, y);
		
		//r1 = ((x * x) + y) / y;

		if(!equivalent(r1, r2))
		{
			++failures;
			
			if(!fd_finite(r1) && fd_finite(r2))
				++incorrect_overflows;
			
			if(failures == 1)
			{
				printf("First failure after %d trials. %f * %f\n", trials, x, y);
			}
		}
		
		++trials;
	}
	
	printf("Total failures: %d out of %d, %d incorrect overflows\n", failures, trials, incorrect_overflows);
}

} // namespace tut

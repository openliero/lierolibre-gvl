#include <tut/tut.hpp>

#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <gvl/math/ieee.hpp>
#include <cmath>
#include <ctime>
#include <limits>
#include <iostream>
#include <iomanip>

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
	return (x == y) || (std::isnan(x) && std::isnan(y));
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


template<>
template<>
void object::test<2>()
{
#if 0
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
			
			if(!std::isfinite(r1) && std::isfinite(r2))
				++incorrect_overflows;
			
			if(failures == 1)
			{
				printf("First failure after %d trials. %f * %f\n", trials, x, y);
			}
		}
		
		++trials;
	}
	
	printf("Total failures: %d out of %d, %d incorrect overflows\n", failures, trials, incorrect_overflows);
#endif
}

template<>
template<>
void object::test<3>()
{
	gvl_init_ieee();
	
	double computedNaN = gSqrt(-1.0);

	double negZero = -10.0 * 0.0;
	double zero = 0.0;

	ensure(std::isnan(computedNaN) != 0);

	ensure("x-x is not optimized", std::isnan(gS(computedNaN, computedNaN)) != 0);
	ensure("x*0.0 is not optimized", std::isnan(gM(computedNaN, 0.0)) != 0);
	ensure("x/x is not optimized", std::isnan(gD(computedNaN, computedNaN)) != 0);
	  
	ensure("dividing by -0 yields -infinity", gD(1.0, negZero) == -std::numeric_limits<double>::infinity());
	ensure("dividing by 0 yields infinity", gD(1.0, zero) == std::numeric_limits<double>::infinity());
	ensure("dividing 0 by 0 yields NaN", std::isnan(gD(zero, zero)) != 0);

	// Fails with round-double-53(round-extended-64(x * 2) / 2)
	ensure("double overflow with multiply", gD(gM(1.7e308, 2.0), 2.0) == std::numeric_limits<double>::infinity());
	// Fails with round-double-53(round-extended-64(x + x) / 2)
	ensure("double overflow with add", gD(gA(1.7e308, 1.7e308), 2.0) == std::numeric_limits<double>::infinity());

	// Fails with round-double-53(round-extended-64(x / 2) * 2)
	ensure("double underflow with divide", gM(gD(4.940656458412e-324, 2.0), 2.0) == 0.0);

	// These fail with round-double-53(round-extended-64(x / y))
	ensure("division is done with double precision #4", gD(1.0800166971961008e+0238, 5.1489795549339881e-0054) == 2.0975354158498831e+0291);
	ensure("division is done with double precision #5", gD(7.3289450094340694e+0083, 1.9115748309497579e-0128) == 3.8339827930213510e+0211);
	ensure("division is done with double precision #6", gD(1.9544203331713143e-0205, 3.5068757973112538e-0184) == 5.5731096455420005e-0022);

	// These fail with round-double-53(round-extended-53(x * y))
	ensure("multiplication is done with double precision #1", gM(8.5782035936114600e-0256, 9.6044770612653327e-0055) == 8.2389159641705354e-0310);
	ensure("multiplication is done with double precision #2", gM(1.5033255271664106e-0171, 7.6121384488969229e-0138) == 1.1443522046551671e-0308);
	ensure("multiplication is done with double precision #3", gM(1.7912119214492215e-0236, 1.0043439961829961e-0072) == 1.7989929391989340e-0308);

	// These fail with round-double-53(round-extended-53(x / y))
	ensure("division is done with double precision #1", gD(9.9174847050126468e-0179, 6.5505123943214619e+0129) == 1.5140013647802517e-0308);
	ensure("division is done with double precision #2", gD(6.0531002917684863e-0252, 6.8672903290096396e-0306) == 8.8143940357352393e+0053);
	ensure("division is done with double precision #3", gD(3.5215930923124385e+0097, 2.6716890617707574e+0068) == 1.3181148744825781e+0029);
	
	// These fail with round-double-53(round-extended-64(sqrt(x))
	ensure("sqrt is done with double precision #1", gSqrt(7.9975425458479251e-0252) == 2.8279926707556945e-0126);
	ensure("sqrt is done with double precision #2", gSqrt(9.1244941815835015e+0183) == 9.5522218261425980e+0091);
	ensure("sqrt is done with double precision #3", gSqrt(1.2549478092296626e-0152) == 1.1202445309974347e-0076);
	
	// These fail with round-double-53(round-extended-64(round-extended-64(1 + x) - y))
	ensure("intermediates are double precision #1", gS(gA(1.0, -11562242321027462.), 1642019876357220.2) == -13204262197384680.);
	ensure("intermediates are double precision #2", gS(gA(1.0, -6.4758762313609902e-006), 4.6367248808744297e-005) == 0.99994715687495983);
	ensure("intermediates are double precision #3", gS(gA(1.0, -0.025106293868442715), -5.4931982728117911e-015) == 0.97489370613156268);
		
}

template<>
template<>
void object::test<4>()
{
#if 0
	unsigned int const flags = _RC_NEAR | _PC_64 /* | _PC_53*/ | _EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT;
    _control87(flags, _MCW_EM | _MCW_PC | _MCW_RC);
    
	gvl::tt800 rnd(2);
	
	double one = 1.0;

	for(unsigned long long i = 0; i < 400000000000ull; ++i)
	{
		double r1, r2;
		double x, y;
		
		FD_LO(x) = rnd();
		FD_HI(x) = rnd();
		FD_LO(y) = rnd();
		FD_HI(y) = rnd();
		
		__asm
		{
			fld1
			fadd x
			fsub y
			fstp r1
		}
		
		__asm
		{
			movsd xmm1, one
			addsd xmm1, x
			subsd xmm1, y
			movsd r2, xmm1
		}

		if(!equivalent(r1, r2))
		{
			printf("FAIL! %.20f\n", x, y);
		}

#if 0
		if(r2 < 1.7e308)
		{
			double ratio = fd_nextafter(std::numeric_limits<double>::max() / r2, 0.0);
			x *= ratio;
		}
		else if(r2 == std::numeric_limits<double>::infinity())
		{
			if((x * 0.5) * y > 0.9e308)
			{
				double ratio = fd_nextafter((0.9e308) / ((x * 0.5) * y), std::numeric_limits<double>::max());
				y *= ratio;
			}
			else
				y = fd_nextafter(y, 0.0);
		}
		else
		{
			x = fd_nextafter(x, std::numeric_limits<double>::max());
		}
#endif

		if((i & 0xffffff) == 0)
		{
			std::cout << "\nTested " << i;
		}

   }
#endif

}

/*

var p1 = { x = 0, y = 0 }
var p2 = { p1 & z = 0 }



x & y

*/

} // namespace tut

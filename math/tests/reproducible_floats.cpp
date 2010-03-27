#include "../../test/test.hpp"

#include "../tt800.hpp"
#include "../../support/algorithm.hpp"
#include "../fdlibm/fdlibm.h"
#include "../ieee.hpp"
#include <cmath>
#include <ctime>
#include <limits>
#include <iostream>
#include <iomanip>

bool equivalent(double x, double y)
{
	return (x == y) || (fd_isnan(x) && fd_isnan(y));
}

GVLTEST_SUITE(gvl, reproducible_floats)

GVLTEST(gvl, reproducible_floats, corner_cases)
{
	gvl_init_ieee();
	
	double computedNaN = gSqrt(-1.0);

	double negZero = -10.0 * 0.0;
	double zero = 0.0;

	ASSERT("isnan(computedNaN)", fd_isnan(computedNaN) != 0);

	ASSERT("x-x is not optimized", fd_isnan(gS(computedNaN, computedNaN)) != 0);
	ASSERT("x*0.0 is not optimized", fd_isnan(gM(computedNaN, 0.0)) != 0);
	ASSERT("x/x is not optimized", fd_isnan(gD(computedNaN, computedNaN)) != 0);
	  
	ASSERT("dividing by -0 yields -infinity", gD(1.0, negZero) == -std::numeric_limits<double>::infinity());
	ASSERT("dividing by 0 yields infinity", gD(1.0, zero) == std::numeric_limits<double>::infinity());
	ASSERT("dividing 0 by 0 yields NaN", fd_isnan(gD(zero, zero)) != 0);

	// Fails with round-double-53(round-extended-64(x * 2) / 2)
	ASSERT("double overflow with multiply", gD(gM(1.7e308, 2.0), 2.0) == std::numeric_limits<double>::infinity());
	// Fails with round-double-53(round-extended-64(x + x) / 2)
	ASSERT("double overflow with add", gD(gA(1.7e308, 1.7e308), 2.0) == std::numeric_limits<double>::infinity());

	// Fails with round-double-53(round-extended-64(x / 2) * 2)
	ASSERT("double underflow with divide", gM(gD(4.940656458412e-324, 2.0), 2.0) == 0.0);

	// These fail with round-double-53(round-extended-64(x / y))
	ASSERT("division is done with double precision #4", gD(1.0800166971961008e+0238, 5.1489795549339881e-0054) == 2.0975354158498831e+0291);
	ASSERT("division is done with double precision #5", gD(7.3289450094340694e+0083, 1.9115748309497579e-0128) == 3.8339827930213510e+0211);
	ASSERT("division is done with double precision #6", gD(1.9544203331713143e-0205, 3.5068757973112538e-0184) == 5.5731096455420005e-0022);

	// These fail with round-double-53(round-extended-53(x * y))
	ASSERT("multiplication is done with double precision #1", gM(8.5782035936114600e-0256, 9.6044770612653327e-0055) == 8.2389159641705354e-0310);
	ASSERT("multiplication is done with double precision #2", gM(1.5033255271664106e-0171, 7.6121384488969229e-0138) == 1.1443522046551671e-0308);
	ASSERT("multiplication is done with double precision #3", gM(1.7912119214492215e-0236, 1.0043439961829961e-0072) == 1.7989929391989340e-0308);

	// These fail with round-double-53(round-extended-53(x / y))
	ASSERT("division is done with double precision #1", gD(9.9174847050126468e-0179, 6.5505123943214619e+0129) == 1.5140013647802517e-0308);
	ASSERT("division is done with double precision #2", gD(6.0531002917684863e-0252, 6.8672903290096396e-0306) == 8.8143940357352393e+0053);
	ASSERT("division is done with double precision #3", gD(3.5215930923124385e+0097, 2.6716890617707574e+0068) == 1.3181148744825781e+0029);
	
	// These fail with round-double-53(round-extended-64(sqrt(x))
	ASSERT("sqrt is done with double precision #1", gSqrt(7.9975425458479251e-0252) == 2.8279926707556945e-0126);
	ASSERT("sqrt is done with double precision #2", gSqrt(9.1244941815835015e+0183) == 9.5522218261425980e+0091);
	ASSERT("sqrt is done with double precision #3", gSqrt(1.2549478092296626e-0152) == 1.1202445309974347e-0076);
	
	// These fail with round-double-53(round-extended-64(round-extended-64(1 + x) - y))
	ASSERT("intermediates are double precision #1", gS(gA(1.0, -11562242321027462.), 1642019876357220.2) == -13204262197384680.);
	ASSERT("intermediates are double precision #2", gS(gA(1.0, -6.4758762313609902e-006), 4.6367248808744297e-005) == 0.99994715687495983);
	ASSERT("intermediates are double precision #3", gS(gA(1.0, -0.025106293868442715), -5.4931982728117911e-015) == 0.97489370613156268);
}

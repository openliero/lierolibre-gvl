#ifndef UUID_FDA91F49FD8F423968AA34AD0C51384B
#define UUID_FDA91F49FD8F423968AA34AD0C51384B

#include "ieee.h"

namespace gvl
{

// C++ specific features
struct rdouble
{
	rdouble(double value)
	: value(value)
	{

	}

	rdouble& operator+=(rdouble b)
	{
		value = gA(value, b.value);
		return *this;
	}

	rdouble& operator-=(rdouble b)
	{
		value = gS(value, b.value);
		return *this;
	}

	rdouble& operator*=(rdouble b)
	{
		value = gM(value, b.value);
		return *this;
	}

	rdouble& operator/=(rdouble b)
	{
		value = gD(value, b.value);
		return *this;
	}

	rdouble operator+(rdouble b) const
	{
		return gA(value, b.value);
	}

	rdouble operator-(rdouble b) const
	{
		return gS(value, b.value);
	}

	rdouble operator*(rdouble b) const
	{
		return gM(value, b.value);
	}

	rdouble operator/(rdouble b) const
	{
		return gD(value, b.value);
	}

	rdouble sqrt() const
	{
		return gSqrt(value);
	}

	double value;
};

}

#endif /* UUID_FDA91F49FD8F423968AA34AD0C51384B */

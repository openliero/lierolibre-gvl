#ifndef UUID_786BDFAEC45D4C3F36D47BB76ABCE5DE
#define UUID_786BDFAEC45D4C3F36D47BB76ABCE5DE

#include "ieee.hpp"

#include <cmath>

namespace gvl
{

template<typename T = double>
struct normal_distribution
{
	typedef T value_type;

	template<typename Random>
	value_type normal_variate(Random rand, value_type mean = value_type(0), value_type sigma = value_type(1))
	{
		using std::sqrt;
		using std::log;
		using std::sin;
		using std::cos;

		if(!valid)
		{
			r1 = value_type(rand.get_double());
			value_type r2 = value_type(rand.get_double());
			cached_rho = sqrt(value_type(-2) * log(value_type(1) - r2));
		}
		else
		{
			valid = false;
		}

		value_type const pi = value_type(3.14159265358979323846);

		return cached_rho * (valid ?
                          cos(value_type(2) * pi * r1) :
                          sin(value_type(2) * pi * r1)) * sigma + mean;
	}

	normal_distribution()
	: valid(false)
	{
	}

	value_type r1, cached_rho;
	bool valid;
};

} // namespace gvl

#endif // UUID_786BDFAEC45D4C3F36D47BB76ABCE5DE

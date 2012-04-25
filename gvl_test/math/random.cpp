#include <tut/tut.hpp>

#include <gvl/math/cmwc.hpp>
#include <gvl/math/normal_distribution.hpp>

namespace tut
{

struct random_data
{
	
};

typedef test_group<random_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("random");
} // namespace

namespace tut
{

template<>
template<>
void object::test<1>()
{
	gvl::normal_distribution<> dist;
	gvl::normal_distribution<gvl::rdouble> rdist;
	
	gvl::mwc rand;
	
	double x = dist.normal_variate(rand, 0.0, 1.0);
	x = rdist.normal_variate(rand, 0.0, 1.0).value;
}

} // namespace tut

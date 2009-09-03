#include <gvl/tut/tut.hpp>

// Template for tut boilerplate

namespace tut
{

struct X_data
{
	
};

typedef test_group<X_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::X");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
	
}


} // namespace tut

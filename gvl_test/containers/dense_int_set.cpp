#include <gvl/tut/tut.hpp>

#include <gvl/dense_int_set.hpp>

namespace tut
{

struct dense_int_set_data
{
	
};

typedef test_group<dense_int_set_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::dense_int_set");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
	gvl::dense_int_set<> s;
	s.insert(200);
	s.insert(1000);
	s.insert(500);
	s.insert(10);
	s.insert(100000);
}


} // namespace tut

#include <gvl/tut/tut.hpp>

#include <gvl/containers/compact_interval_set.hpp>
#include <gvl/math/cmwc.hpp>
#include <functional>
#include <memory>
#include <algorithm>

// For bounded_rectangle_packer
#include <gvl/math/rect.hpp>
#include <gvl/containers/list.hpp>
#include <vector>

namespace tut
{

struct cis_data
{
	
};

typedef test_group<cis_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::compact_interval_set");
} // namespace

namespace tut
{

template<>
template<>
void object::test<1>()
{
	gvl::compact_interval_set<int> s;
	
	s.insert_no_overlap(0, 100);   // [0, 100)
	s.insert_no_overlap(150, 200); // [0, 100) [150, 200)
	s.insert_no_overlap(100, 150); // [0, 200)
	ensure("ranges merged after insert", s.end() - s.begin() == 1);
	s.erase_no_overlap(50, 150);  // [0, 50) [150, 200)
	ensure("range split in two", s.end() - s.begin() == 2);
	s.erase_no_overlap(0, 50);    // [150, 200)
	ensure("range erased", s.end() - s.begin() == 1);
	s.erase_no_overlap(150, 175); // [175, 200)
	ensure("range cut at front", (s.end() - s.begin() == 1) && s.begin()->begin == 175);
	s.erase_no_overlap(190, 200); // [175, 190)
	ensure("range cut at back", (s.end() - s.begin() == 1) && s.begin()->end == 190);
	s.insert_no_overlap(0, 100);   // [0, 100) [175, 190)
	s.insert_no_overlap(100, 175); // [0, 190)
	ensure("three ranges merged", (s.end() - s.begin() == 1) && s.begin()->begin == 0);
}


} // namespace tut

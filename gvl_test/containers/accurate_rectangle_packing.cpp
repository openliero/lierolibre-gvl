#include <gvl/tut/tut.hpp>

#include <gvl/containers/accurate_rectangle_packing.hpp>
#include <gvl/math/cmwc.hpp>
#include <functional>
#include <memory>
#include <algorithm>


namespace tut
{

struct arp_data
{
	
};

typedef test_group<arp_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::accurate_rectangle_packing");
} // namespace

namespace tut
{

template<>
template<>
void object::test<1>()
{
	typedef gvl::accurate_rectangle_packing packer_t;
	packer_t packing(70, 70);
	gvl::mwc rand(1);
	
	std::vector<std::pair<int, int> > rects;
	for(int i = 200; i >= 1; --i)
	{
		int w = rand(4, 10);
		int h = rand(4, 10);
		rects.push_back(std::make_pair(w, h));
	}
	
	int box_count = 0;
	int area = 0;
	
	std::vector<gvl::rect> packed_rects;
	
	for(std::size_t i = 0; i < rects.size(); ++i)
	{
		std::pair<int, int>& r = rects[i];
		
		packer_t::rect_handle h = packing.try_fit(r.first, r.second, true);
		
		if(packing.is_valid(h))
		{
			++box_count;
			area += r.first * r.second;
			packed_rects.push_back(packing.get_rect(h));
		}
	}
	
#if 0
	int cur_free = packing.free_rects.size();
	std::cout << cur_free << " free rectangles\n";

	std::cout << box_count << " boxes, area: " << area << ", "
	<< (area * 100.0 / (double(packing.width) * double(packing.height))) << "%" << std::endl;
#endif
}


} // namespace tut

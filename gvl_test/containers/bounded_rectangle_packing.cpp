#include <tut/tut.hpp>

#include <gvl/containers/bounded_rectangle_packing.hpp>
#include <gvl/containers/accurate_rectangle_packing.hpp>
#include <gvl/math/vec.hpp>
#include <gvl/math/cmwc.hpp>
#include <vector>
#include <sstream>
#include <algorithm>

#include <flip/qv/qv.hpp>

#define GVL_PROFILE 1
#if GVL_PROFILE
#include <gvl/support/profile.hpp>
#endif

namespace tut
{

struct brp_data
{
	
};

typedef test_group<brp_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::recursive_bounded_rectangle_packing");
} // namespace

namespace tut
{


template<typename Packer>
struct packing_trial
{
	typedef Packer packer_t;
	typedef typename packer_t::rect_handle rect_handle;
	
	packing_trial(std::string const& name, int width, int height)
	: name(name)
	, width(width), height(height)
	, packing(width, height)
	, trials(0)
	, packed_count(0)
	, area(0)
	{
	}
	
	void try_fit(std::vector<gvl::ivec2> const& boxes, bool abort_on_first_fail = false)
	{
		for(std::size_t i = 0; i < boxes.size(); ++i)
		{
			++trials;
			int w = boxes[i].x;
			int h = boxes[i].y;
			rect_handle n = packing.try_fit(w, h, true);

			if(packing.is_valid(n))
			{
				packed_rects.push_back(n);
				area += w*h;
				++packed_count;
			}
			else if(abort_on_first_fail)
			{
				break;
			}
		}
	}
	
	std::string get_stats()
	{
		std::stringstream ss;
		ss << name << ": " << (100.0 * area / (width*height)) << "%, " << packed_count << " boxes out of " << trials << " trials";
		return ss.str();
	}
	
	void render(double x, double y, double scale = 4.0)
	{
		using namespace qv;
		for(std::size_t i = 0; i < packed_rects.size(); ++i)
		{
			gvl::rect const& r = packing.get_rect(packed_rects[i]);
			drawBox(x + r.x1 * scale, y + r.y1 * scale, r.width() * scale, r.height() * scale, colorize(&r));
		}
	}
	
	std::string name;
	int width, height;
	packer_t packing;
	int trials;
	int packed_count;
	int area;
	
	std::vector<rect_handle> packed_rects;
};

struct by_area
{
	bool operator()(gvl::ivec2 const& a, gvl::ivec2 const& b) const
	{
		return a.x*a.y > b.x*b.y;
	}
};

template<>
template<>
void object::test<1>()
{	
	gvl::mwc rand(1);
	
	std::cout << "Packing rectangles..." << std::endl;
	
	packing_trial<gvl::recursive_bounded_rectangle_packing> rtrial("recursive", 256, 256);
	packing_trial<gvl::accurate_rectangle_packing> gtrial("maximal space", 256, 256);
	
	std::vector<gvl::ivec2> rects;
	
	for(int i = 0; i < 1500; ++i)
	{
#if 0
		int w = 3 + (1 << rand(0, 5)); //5 + int(std::pow(rand.get_double() * 10.0, 2));
		int h = 3 + (1 << rand(0, 5)); //5 + int(std::pow(rand.get_double() * 10.0, 2));
#else
		//int w = 5 + int(std::pow(rand.get_double() * 3.0, 3));
		//int h = 5 + int(std::pow(rand.get_double() * 3.0, 3));
		int r = rand(1, 4);
		int w = 3 + r*r, h = w;
#endif
		rects.push_back(gvl::ivec2(w, h));
	}
	
	//std::sort(rects.begin(), rects.end(), by_area());
	
	{
		GVL_PROF_TIMER("recursive");
		rtrial.try_fit(rects, false);
	}
	{
		GVL_PROF_TIMER("maximal space");
		gtrial.try_fit(rects, false);
	}
	
	std::cout << "Done." << std::endl;
	
	std::stringstream ss;
	gvl::present_profile(ss);
	
	std::vector<std::string> lines;
	std::string line;
	while(std::getline(ss, line))
	{
		lines.push_back(line);
	}
	
	int cur_free = gtrial.packing.free_rects.size();
	std::cout << cur_free << " free rectangles\n";

	do
	{
		double scale = 1.5;
		rtrial.render(0, 0, scale);
		gtrial.render(800.0 - 256.0 * scale, 0, scale);
		
		qv::drawText(rtrial.get_stats(), 20.0, 256.0 * scale + 40.0);
		qv::drawText(gtrial.get_stats(), 20.0, 256.0 * scale + 60.0);
		
		for(std::size_t i = 0; i < lines.size(); ++i)
		{
			qv::drawText(lines[i], 20.0, 256.0 * scale + 90.0 + i*20.0, qv::Color(255, 255, 128, 255));
		}
	}
	while(qv::show());
	
#if GVL_PROFILE
	gvl::present_profile(std::cout);
#endif
}


} // namespace tut

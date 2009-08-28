#include <gvl/tut/tut.hpp>

#include <gvl/containers/compact_interval_set.hpp>
#include <gvl/math/cmwc.hpp>
#include <gvl/support/algorithm.hpp>
#include <gvl/support/macros.hpp>
#include <functional>
#include <memory>
#include <algorithm>

#include <gvl/tut/quickcheck/context.hpp>
#include <gvl/tut/quickcheck/generator.hpp>
#include <gvl/tut/quickcheck/property.hpp>

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

struct bounded_rectangle_packer
{
	struct stripe
	{
		typedef std::vector<gvl::rect> rectangle_iterator;
		typedef gvl::compact_interval_set<unsigned int> gap_set;
		
		stripe(
			unsigned int width,
			unsigned int stripe_y1_init,
			unsigned int stripe_y2_init)
		: covered_width(0)
		, stripe_y1(stripe_y1_init)
		, stripe_y2(stripe_y2_init)
		, extent_y1(0)
		, extent_y1(-1)
		{
			gaps.insert_no_overlap(0, width);
		}
		
		void recompute_vertical_extents()
		{
			std::vector<gvl::rect>::iterator i = rectangles.begin(), e = rectangles.end();
			
			int maxy = 0;
			int miny = 0;
			for(; i != e; ++i)
			{
				int y1 = i->y1;
				int y2 = i->y2;
				if(y2 > maxy)
					maxy = y2;
				if(y1 < miny)
					miny = y1;
			}
			
			extent_y1_ = miny;
			extent_y2_ = maxy;
		}
		
		int extent_y1()
		{
			if(extent_y1_ < 0)
				recompute_vertical_extents();
			
			return extent_y1_;
		}
		
		int extent_y2()
		{
			if(extent_y2_ < 0)
				recompute_vertical_extents();
			
			return extent_y2_;
		}
		
		int extent_height()
		{
			return extent_y2() - extent_y1();
		}
		
		void erase(rectangle_iterator i)
		{
			int x1 = i->x1;
			int x2 = i->x2;
			
			if(i->y1 == extent_y1_)
				extent_y1_ = -1;
			if(i->y2 == extent_y2_)
				extent_y2_ = -1;
			
			covered_width -= (x2 - x1);
			gaps.insert(x1, x2);
			rectangles.erase(i);
		}
		
		// Less is better
		int space_penalty(int left, int goal_width)
		{
			return left >= goal_width ?
				(left - goal_width) : (goal_width - left) * 3;
		}
		
		struct proposal
		{
			proposal()
			: valid(false)
			{
			}
			
			proposal(gap_set::iterator gap, int penalty)
			: valid(true)
			, penalty(penalty)
			, gap(gap)
			{
			}
			
			bool valid;
			int penalty;
			gap_set::iterator gap;
		};
		
		proposal fit(int rect_width, int rect_height, bool allow_rotate)
		{
			if(allow_rotate
			&& rect_height < rect_width
			&& rect_width <= stripe_y2 - stripe_y1)
			{
				// We can fit it better rotated
				std::swap(rect_width, rect_height);
			}
		
			int max_available_space = width - covered_width;
			if(max_available_space < rect_width)
			{
				// No way it will fit
				return proposal();
			}
			
			// We will try to leave space for another rectangle
			// with size rect_width.
			
			gap_set::iterator best_fit = gaps.end();
			int best_fit_space_left = -1;
			
			for(gap_set::iterator i = gaps.begin(); i != gaps.end(); ++i)
			{
				int space_left = (i->end - i->begin) - rect_width;
				
				if(space_left >= 0)
				{
					if(best_fit_space_left < 0)
					{
						best_fit = i;
						best_fit_space_left = space_left;
					}
					else
					{
						int new_penalty = space_penalty(space_left, rect_width);
						int best_penalty = space_penalty(best_fit_space_left, rect_width);
						
						if(new_penalty < best_penalty)
						{
							best_fit = i;
							best_fit_space_left = space_left;
						}
					}
				}
			}
			
			if(best_fit_space_left < 0)
			{
				// Fits nowhere
				return proposal();
			}
			
			int penalty = space_penalty(best_fit_space_left, rect_width) * (stripe_y2 - stripe_y1);
			
			return proposal(best_fit, penalty);
		}
		
		rectangle_iterator accept(proposal p, int rect_width, int rect_height)
		{
			int x1 = p.gap->begin;
			int x2 = x1 + rect_width;
			int y1 = stripe_y1;
			int y2 = y1 + rect_height;
			
			rectangles.push_back(gvl::rect(x1, y1, x2, y2));
			gaps.insert_no_overlap(x1, x2);
			covered_width -= rect_width;
			if(extents_invalid() || y1 < extent_y1_)
				extent_y1_ = y1; // If extents were invalid, they will remain so
			if(extents_invalid() || y2 > extent_y2_)
				extent_y2_ = y2;
			
			return rectangles.end() - 1;
		}
		
		bool extents_invalid()
		{
			return extent_y2_ < 0;
		}
		
		rectangle_iterator end_rects()
		{
			return rectangles.end();
		}
		
		int covered_width;
		int stripe_y1, stripe_y2;
		int extent_y1_, extent_y2_; // Used vertical extents. Unknown when less than 0.
		//int width;
		
		gap_set gaps;
		std::vector<gvl::rect> rectangles;
	};
	
	typedef std::list<stripe> stripe_iterator;
	
	struct fit_result
	{
		fit_result()
		: valid(false)
		{
		}
		
		fit_result(stripe::rectangle_iterator rect_iter, stripe_iterator stripe_iter)
		: valid(false)
		{
		}
		
		bool valid;
		stripe::rectangle_iterator rect_iter;
		stripe_iterator stripe_iter;
	};
	
	fit_result fit(int rect_width, int rect_height)
	{
		proposal best_p;
		stripe_iterator best_i;
		
		// TODO: Check first if it's better to split
		
		for(stripe_iterator i = stripes.begin();
		  i != stripes.end();
		  ++i)
		{
			proposal p = i->fit(rect_width, rect_height, true);
			
			if(!best_p.valid
			|| p.penalty < best_p.penalty)
			{
				best_p = p;
				best_i = i;
				if(p.penalty == 0)
					break;
			}
		}
		
		if(best_p.valid)
		{
			stripe::rectangle_iterator rect_iter = best_i->accept(best_p, rect_width, rect_height);
			return fit_result(rect_iter, best_i);
		}
		
		{
			// TODO: Try splitting or resizing stripes
			return fit_result();
		}
	}
	
	std::list<stripe> stripes;
};

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

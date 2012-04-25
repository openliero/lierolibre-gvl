#ifndef GVL_ACCURATE_RECTANGLE_PACKING_HPP
#define GVL_ACCURATE_RECTANGLE_PACKING_HPP

#include "../math/rect.hpp"
#include "../support/debug.hpp"
#include <list>
#include <utility>

namespace gvl
{

template<typename X, typename Y>
std::pair<X, Y> operator+(std::pair<X, Y> const& a, std::pair<X, Y> const& b)
{
	return std::pair<X, Y>(a.first + b.first, a.second + b.second);
}

struct accurate_rectangle_packing
{
	typedef std::list<gvl::rect> rect_list;
	typedef rect_list::iterator rect_handle;

	accurate_rectangle_packing(int width, int height)
	: width(width), height(height)
	{
		clear();
	}

	typedef std::pair<int, int> distance_t;

	std::pair<int, int> distance(gvl::ivec2 const& a, gvl::ivec2 const& b)
	{
		std::pair<int, int> p(std::abs(a.x - b.x), std::abs(a.y - b.y));

		if(p.second < p.first)
			std::swap(p.first, p.second);
		return p;
	}

	template<typename T>
	T min4(T const& a, T const& b, T const& c, T const& d)
	{
		T const& x = a < b ? a : b;
		T const& y = c < d ? c : d;
		return x < y ? x : y;
	}

	rect_handle try_fit(int w, int h, bool allow_rotate)
	{
		rect_handle best_rect = free_rects.end();
		int best_dir;
		distance_t min_dist(0xffffff, 0);

		for(rect_handle i = free_rects.begin(); i != free_rects.end(); ++i)
		{
			gvl::rect& r = *i;

			if((r.width() >= w && r.height() >= h)
			|| (allow_rotate && r.width() >= h && r.height() >= w))
			{
				distance_t ul = distance(r.ul(), gvl::ivec2(0, 0));
				distance_t ur = distance(r.ur(), gvl::ivec2(width, 0));
				distance_t lr = distance(r.lr(), gvl::ivec2(width, height));
				distance_t ll = distance(r.ll(), gvl::ivec2(0, height));

				int dir;
				distance_t dist;

				dist = ll; dir = 0;
				if(lr < dist)
				{ dist = lr; dir = 1; }
#if 0
				if(ul < dist)
				{ dist = ul; dir = 2; }
				if(ur < dist)
				{ dist = ur; dir = 3; }
#endif


				if(dist < min_dist)
				{
					min_dist = dist;
					best_rect = i;
					best_dir = dir;
				}
			}
		}

		if(best_rect == free_rects.end())
			return used_rects.end();

		if(allow_rotate)
		{
			int space_w = best_rect->width();
			int space_h = best_rect->height();

			distance_t non_rot = distance(gvl::ivec2(space_w, space_h), gvl::ivec2(w, h));
			distance_t rot = distance(gvl::ivec2(space_w, space_h), gvl::ivec2(h, w));

			if(best_rect->width() < w || best_rect->height() < h)
				std::swap(w, h);
			else if(best_rect->width() < h || best_rect->height() < w)
				/* cannot rotate */;
			else if(rot < non_rot)
				std::swap(w, h);
		}

		gvl::rect r;

		bool above = best_dir >= 2; // best_rect->center_y() < height/2;
		bool left = (best_dir & 1) == 0; // best_dirbest_rect->center_x() < width/2;

		if(left)
		{
			if(above)
				r = gvl::rect(best_rect->x1, best_rect->y1, best_rect->x1 + w, best_rect->y1 + h);
			else
				r = gvl::rect(best_rect->x1, best_rect->y2 - h, best_rect->x1 + w, best_rect->y2);
		}
		else
		{
			if(above)
				r = gvl::rect(best_rect->x2 - w, best_rect->y1, best_rect->x2, best_rect->y1 + h);
			else
				r = gvl::rect(best_rect->x2 - w, best_rect->y2 - h, best_rect->x2, best_rect->y2);
		}
		cut(r);

		used_rects.push_front(r);
		return used_rects.begin();
	}

	bool is_valid(rect_handle h)
	{
		return h != used_rects.end();
	}

	gvl::rect const& get_rect(rect_handle h)
	{
		return *h;
	}

	void cut(gvl::rect r)
	{
		for(rect_handle i = free_rects.begin(); i != free_rects.end();)
		{
			gvl::rect hole(r);
			gvl::rect solid(*i);
			if(hole.intersect(solid))
			{
				i = free_rects.erase(i);

				add_free_rect(solid.x1, solid.y1, solid.x2, hole .y1);
				add_free_rect(solid.x1, hole .y2, solid.x2, solid.y2);

				add_free_rect(solid.x1, solid.y1, hole .x1, solid.y2);
				add_free_rect(hole .x2, solid.y1, solid.x2, solid.y2);

				continue;
			}

			++i;
		}

		process_insert_queue();
	}

	bool add_free_rect(int x1, int y1, int x2, int y2)
	{
		if(x1 != x2 && y1 != y2)
		{
			insert_queue.push_back(gvl::rect(x1, y1, x2, y2));
			return true;
		}

		return false;
	}

	void process_insert_queue()
	{
		for(rect_handle q = insert_queue.begin(); q != insert_queue.end();)
		{
			gvl::rect& r = *q;

			int accum_change_mask;

			do
			{
				accum_change_mask = 0;

				for(rect_handle i = free_rects.begin(); i != free_rects.end();)
				{
					int change_mask = i->maximal_extend(r);
					accum_change_mask |= change_mask;

					if(*i == r)
					{
						// Discard *i because it's the same as r
						i = free_rects.erase(i);
						continue;
					}
					else if(change_mask & 1) // *i was changed
					{
						// Must reprocess *i because it changed
						rect_handle curi = i;
						++i;
						// NOTE: Important to insert at the end, so we don't skip it
						insert_queue.splice(insert_queue.end(), free_rects, curi);
						continue;
					}

					++i;
				}
			}
			while(accum_change_mask & 2); // While r changed, repeat

			// Processed, insert

			// NOTE: We insert at the end so the processed
			// rectangle can be e.g. cut again if necessary
			// (when cut is the caller).
			rect_handle curq = q;
			++q;
			free_rects.splice(free_rects.end(), insert_queue, curq);
		}

		sassert(insert_queue.empty());
	}

	void clear()
	{
		used_rects.clear();
		free_rects.clear();
		free_rects.push_back(gvl::rect(0, 0, width, height));
	}

	std::list<gvl::rect> free_rects;
	std::list<gvl::rect> used_rects;

	std::list<gvl::rect> insert_queue;
	int width, height;
};

}

#endif // GVL_ACCURATE_RECTANGLE_PACKING_HPP

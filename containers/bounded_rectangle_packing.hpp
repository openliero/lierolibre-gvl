#ifndef UUID_E2BFCF9F131A4133528A3FB16B022FC6
#define UUID_E2BFCF9F131A4133528A3FB16B022FC6

#include <gvl/math/rect.hpp>
#include <gvl/support/debug.hpp>
#include <memory>
#include <utility>

#include <map>
#include <list>

namespace gvl
{

struct recursive_bounded_rectangle_packing
{
	struct node
	{
		friend struct recursive_bounded_rectangle_packing;

		node(node* parent, int x1, int y1, int x2, int y2)
		: occupied(false)
		, splittable(false)
		, parent(parent)
		, largest_free_width(x2 - x1)
		, largest_free_height(y2 - y1)
		, rect_(x1, y1, x2, y2)
		, enclosing_(x1, y1, x2, y2)
		{
		}

		bool may_fit(int w, int h, bool allow_rotate)
		{
			bool ret = (w <= largest_free_width && h <= largest_free_height);
			ret = ret || (allow_rotate && h <= largest_free_width && w <= largest_free_height);
			return ret;
		}

		node* try_fit(int w, int h, bool allow_rotate)
		{
			if(!may_fit(w, h, allow_rotate))
				return 0;

			if(splittable)
			{
				sassert(occupied);
				// TODO: may_fit may return true even if the rectangle doesn't
				// fit, no matter how we split. We should test this case before
				// splitting.

				int cur_w = rect_.width();
				int cur_h = rect_.height();
				int enc_w = enclosing_.width();
				int enc_h = enclosing_.height();
				int space_w = (enc_w - cur_w);
				int space_h = (enc_h - cur_h);

				// TODO: Should we split so that the new rectangle fits?
				bool cut_horiz;

#if 0
				if(((h > cur_h   || w > space_w) && (w > cur_h   || h > space_w))
				|| ((h > space_h || w > enc_w  ) && (w > space_h || h > enc_w  )))
					cut_horiz = false; // ...must cut vertically (if anything)
				else if(((h > space_h || w > cur_w  ) && (w > space_h || h > cur_w  ))
				     || ((h > enc_h   || w > space_w) && (w > enc_h   || h > space_w)))
					cut_horiz = true; // ...must cut horizontally
				else
#endif
				int diff_w = std::abs(std::max(w, h) - cur_w);
				int diff_h = std::abs(std::max(w, h) - cur_h);

				cut_horiz = space_w < space_h;
				//cut_horiz = diff_h > diff_w;

				if(cut_horiz)
				{
					if(enclosing_.x1 + cur_w != enclosing_.x2)
						children[0].reset(new node(this, enclosing_.x1 + cur_w, enclosing_.y1, enclosing_.x2, enclosing_.y1 + cur_h));
					children[1].reset(new node(this, enclosing_.x1, enclosing_.y1 + cur_h, enclosing_.x2, enclosing_.y2));
				}
				else
				{
					if(enclosing_.y1 + cur_h != enclosing_.y2)
						children[0].reset(new node(this, enclosing_.x1, enclosing_.y1 + cur_h, enclosing_.x1 + cur_w, enclosing_.y2));
					children[1].reset(new node(this, enclosing_.x1 + cur_w, enclosing_.y1, enclosing_.x2, enclosing_.y2));
				}

				// largest_free_width/largest_free_height does not need to be changed here if
				// propagate_largest_splittable has done its job correctly.
				splittable = false;
			}

			node* ch0 = children[0].get();
			node* ch1 = children[1].get();

			if(ch1) // ch1 is always set if ch0 or ch1 is set
			{
				if(ch0)
				{
					node* n = ch0->try_fit(w, h, allow_rotate);
					if(n) return n;
				}
				return ch1->try_fit(w, h, allow_rotate);
			}

			if(occupied)
				return 0;

			// may_fit is accurate for leaves, so we know it will fit at this point.
			// We just need to find out the best orientation.
			if(allow_rotate && should_rotate_(largest_free_width, largest_free_height, w, h))
				std::swap(w, h);

			return known_fit_(w, h);
		}

		void make_empty()
		{
			sassert(occupied);
			occupied = false;

			propagate_empty_(); // 'this' is not occupied
		}

		bool empty()
		{
			return leaf() && !occupied;
		}

	private:
		static bool should_rotate_(int cw, int ch, int w, int h)
		{
			if(w > ch || h > cw)
				return false; // Cannot rotate
			else if(h > ch || w > cw)
				return true; // Must rotate

			// Try to counter-act the shape of the container (making the
			// uncovered parts more square).
			return (cw < ch) == (w < h);
		}

		node* known_fit_(int w, int h)
		{
			sassert(w <= largest_free_width && h <= largest_free_height);
			sassert(leaf());
			sassert(!occupied && !splittable);

			occupied = true;

			// Place rectangle
			rect_.x1 = enclosing_.x1;
			rect_.y1 = enclosing_.y1;
			rect_.x2 = enclosing_.x1 + w;
			rect_.y2 = enclosing_.y1 + h;

			if(w == largest_free_width && h == largest_free_height)
			{
				// Perfect fit
				propagate_largest_(0, 0);
				return this;
			}

			splittable = true;
			propagate_largest_splittable_();
			return this;
		}

		void propagate_largest_(int new_width, int new_height)
		{
			largest_free_width = new_width;
			largest_free_height = new_height;
			if(parent)
				parent->propagate_largest_non_leaf_();
		}

		void propagate_largest_only_()
		{
			if(parent)
				parent->propagate_largest_non_leaf_();
		}

		void propagate_largest_non_leaf_()
		{
			sassert(!leaf());

			if(!children[1].get())
				propagate_largest_(children[0]->largest_free_width, children[0]->largest_free_height);
			else if(!children[0].get())
				propagate_largest_(children[1]->largest_free_width, children[1]->largest_free_height);
			else
			{
				int new_width = std::max(
					children[0]->largest_free_width,
					children[1]->largest_free_width);
				int new_height = std::max(
					children[0]->largest_free_height,
					children[1]->largest_free_height);

				propagate_largest_(new_width, new_height);
			}
		}

		void set_largest_(int new_width, int new_height)
		{
			largest_free_width = new_width;
			largest_free_height = new_height;
		}

		void set_largest_splittable_()
		{
			sassert(leaf() && splittable && occupied);

			if(enclosing_.x2 == rect_.x2) // Only bottom half empty
				set_largest_(enclosing_.width(), enclosing_.y2 - rect_.y2);
			else if(enclosing_.y2 == rect_.y2) // Only right half empty
				set_largest_(enclosing_.x2 - rect_.x2, enclosing_.height());
			else
				set_largest_(enclosing_.width(), enclosing_.height());
		}

		void set_largest_empty_()
		{
			sassert(empty());
			set_largest_(enclosing_.width(), enclosing_.height());
		}

		void propagate_largest_splittable_()
		{
			set_largest_splittable_();
			propagate_largest_only_();
		}

		void propagate_largest_()
		{
			if(leaf())
			{
				if(splittable)
					propagate_largest_splittable_();
				else if(occupied)
					propagate_largest_(0, 0);
				else
					propagate_largest_(rect_.width(), rect_.height());
			}
			else
				propagate_largest_non_leaf_();
		}

		bool leaf()
		{
			return !children[0].get() && !children[1].get();
		}

		bool mergable()
		{
			if((!children[0].get() || children[0]->empty())
			&& (!children[1].get() || children[1]->empty()))
				return true;
			return false;
		}

		void merge_children_()
		{
			sassert(mergable());
			children[0].reset();
			children[1].reset();
			if(occupied)
			{
				splittable = true;
				set_largest_splittable_();
			}
			else
			{
				set_largest_empty_();
			}
			if(parent)
				parent->propagate_empty_(); // parent is not a leaf
		}

		void propagate_empty_()
		{
			if(mergable())
				return merge_children_();
			propagate_largest_non_leaf_();
		}

		bool occupied;
		bool splittable;
		node* parent;
		std::auto_ptr<node> children[2];
		int largest_free_width;
		int largest_free_height;
		gvl::rect enclosing_;
		gvl::rect rect_;
	};

	typedef node* rect_handle;

	recursive_bounded_rectangle_packing(int width, int height)
	: root(new node(0, 0, 0, width, height))
	{
	}

	rect_handle try_fit(int w, int h, bool allow_rotate)
	{
		return root->try_fit(w, h, allow_rotate);
	}

	gvl::rect const& get_rect(rect_handle h)
	{
		return h->rect_;
	}

	bool is_valid(rect_handle h)
	{
		return !!h;
	}

	void erase(rect_handle n)
	{
		n->make_empty();
	}

	// NOTE! This invalidates all rect_handles returned from any try_fit call.
	void clear()
	{
		root->children[0].reset();
		root->children[1].reset();
		root->propagate_largest_();
	}

	std::auto_ptr<node> root;
};

#if 0
struct accurate_rectangle_packing
{
	typedef std::list<gvl::rect> rectangle_list;
	typedef rectangle_list::iterator rect_handle;

	typedef std::map<int, int> line_map;

	accurate_rectangle_packing(int w, int h)
	: width(w), height(h)
	{
		add_vline(0);
		add_vline(w);
		add_hline(0);
		add_hline(h);
	}

	bool is_valid_pos(gvl::rect const& r)
	{
		if(r.x1 < 0 || r.y1 < 0 || r.x2 > width || r.y2 > height)
			return false;

		for(rect_handle i = rectangles.begin(); i != rectangles.end(); ++i)
		{
			if(r.proper_intersecting(*i))
				return false;
		}

		return true;
	}

	rect_handle add(gvl::rect const& r)
	{
		add_vline(r.x1);
		add_vline(r.x2);
		add_hline(r.y1);
		add_hline(r.y2);
		rectangles.push_front(r);
		return rectangles.begin();
	}

	rect_handle try_fit(int x, int y, int w, int h)
	{
		gvl::rect r(x, y, x + w, y + h);
		if(is_valid_pos(r)) return add(r);
		r = gvl::rect(x - w, y, x, y + h);
		if(is_valid_pos(r)) return add(r);
		r = gvl::rect(x, y - h, x + w, y);
		if(is_valid_pos(r)) return add(r);
		r = gvl::rect(x - w, y - h, x, y);
		if(is_valid_pos(r)) return add(r);

		return rectangles.end();
	}

	rect_handle try_fit(int w, int h, bool allow_rotate)
	{
		line_map::iterator yi = hlines.begin();

		for(; yi != hlines.end(); ++yi)
		{
			line_map::iterator xi = vlines.begin();
			int y = yi->first;

			for(; xi != vlines.end(); ++xi)
			{
				int x = xi->first;

				rect_handle n = try_fit(x, y, w, h);
				if(n != rectangles.end()) return n;
				n = try_fit(x, y, h, w);
				if(n != rectangles.end()) return n;
			}
		}

		return rectangles.end();
	}

	gvl::rect const& get_rect(rect_handle h)
	{
		return *h;
	}

	bool is_valid(rect_handle h)
	{
		return h != rectangles.end();
	}

	void erase(rect_handle h)
	{
		remove_line(h->x1, vlines);
		remove_line(h->x2, vlines);
		remove_line(h->y1, hlines);
		remove_line(h->y2, hlines);
		rectangles.erase(h);
	}

	void add_vline(int x)
	{ ++vlines[x]; }

	void add_hline(int y)
	{ ++hlines[y]; }

	void remove_line(int v, line_map& m)
	{
		line_map::iterator i = m.find(v);
		sassert(i != m.end());

		if(--i->second == 0)
			m.erase(i);
	}

	void clear()
	{
		rectangles.clear();
		vlines.clear();
		hlines.clear();

		add_vline(0);
		add_vline(width);
		add_hline(0);
		add_hline(height);
	}

	std::map<int, int> vlines, hlines;
	rectangle_list rectangles;
	int width, height;

	int
};
#endif

#define ROTATE 1

struct gosu_rectangle_packing
{
	typedef std::list<gvl::rect> rectangle_list;
	typedef rectangle_list::iterator rect_handle;

	gosu_rectangle_packing(int w, int h)
	: width(w), height(h)
	, first_x(0), first_y(0)
	, max_w(w), max_h(h)
	{
	}

	bool is_valid_pos(gvl::rect const& r)
	{
		if(r.x1 < 0 || r.y1 < 0 || r.x2 > width || r.y2 > height)
			return false;

		for(rect_handle i = rectangles.begin(); i != rectangles.end(); ++i)
		{
			if(r.proper_intersecting(*i))
				return false;
		}

		return true;
	}

	rect_handle add(gvl::rect const& r)
	{
		first_x += r.width();
        if (first_x + r.width() >= width)
        {
            first_x = 0;
            first_y += r.height();
        }

		rectangles.push_front(r);
		return rectangles.begin();
	}

	rect_handle try_fit(int w, int h, bool allow_rotate)
	{
		if((w > max_w || h > max_h)
#if ROTATE
		&& (w > max_h || h > max_w)
#endif
		)
		{
			return rectangles.end();
		}

		gvl::rect r(first_x, first_y, first_x + w, first_y + h);
#if ROTATE
		gvl::rect r2(first_x, first_y, first_x + h, first_y + w);
#endif

		if(is_valid_pos(r))
			return add(r);
#if ROTATE
		else if(is_valid_pos(r2))
			return add(r2);
#endif

		for (int y = 0; y <= height - h; y += 16)
        for (int x = 0; x <= width - w; x += 8)
        {
#if ROTATE
			r = gvl::rect(x, y, x + h, y + w);
            r2 = gvl::rect(x, y, x + w, y + h);
            if(is_valid_pos(r2))
				r = r2;
			else if(!is_valid_pos(r)) continue;
#else
			r = gvl::rect(x, y, x + w, y + h);
			if(!is_valid_pos(r)) continue;
#endif

            // Found a nice place!

            // Try to make up for the large for()-stepping.
            while (r.y1 > 0 && is_valid_pos(gvl::rect(r.x1, r.y1 - 1, r.x2, r.y2 - 1)))
            { --r.y1; --r.y2; }
            while (r.x1 > 0 && is_valid_pos(gvl::rect(r.x1 - 1, r.y1, r.x2 - 1, r.y2)))
            { --r.x1; --r.x2; }

            return add(r);
        }

        max_w = w - 1;
        max_h = h - 1;

		return rectangles.end();
	}

	gvl::rect const& get_rect(rect_handle h)
	{
		return *h;
	}

	bool is_valid(rect_handle h)
	{
		return h != rectangles.end();
	}

	void erase(rect_handle h)
	{
		rectangles.erase(h);
	}

	void clear()
	{
		first_x = 0;
		first_y = 0;
		max_w = width;
        max_h = height;
		rectangles.clear();
	}

	rectangle_list rectangles;
	int width, height;
	int first_x, first_y;
	int max_w, max_h;
};

}

#endif // UUID_E2BFCF9F131A4133528A3FB16B022FC6

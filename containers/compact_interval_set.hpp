#ifndef UUID_E8CD4134467C409B367DA595A15C7D89
#define UUID_E8CD4134467C409B367DA595A15C7D89

#include <vector>
#include <stdexcept>
#include "../support/debug.hpp"

namespace gvl
{

template<typename T>
struct compact_interval_set
{
	struct range
	{
		range(T begin, T end)
		: begin(begin), end(end)
		{
		}
		
		bool operator==(range const& b) const
		{ return begin == b.begin && end == b.end; }
		
		bool operator!=(range const& b) const
		{ return !operator==(b); }
		
		T begin, end;
	};
	
	typedef typename std::vector<range>::iterator range_iterator;
	
	// Amortized O(N), N = number of ranges
	void insert_no_overlap(T begin, T end)
	{
		for(range_iterator i = ranges.begin(); i != ranges.end(); ++i)
		{
			sassert(i->begin != i->end);
			
			if(i->end == begin)
			{
				i->end = end;
				extend_forwards_(i, end);
				return;
			}
			else if(i->begin == end)
			{
				i->begin = begin;
				extend_backwards_(i, begin);
				return;
			}
		}
		
		// Isolated range
		ranges.push_back(range(begin, end));
	}
	
	// O(N), N = number of ranges
	void erase_no_overlap(T hole_begin, T hole_end)
	{
		for(range_iterator i = ranges.begin(); i != ranges.end(); ++i)
		{
			T begin = i->begin;
			T end = i->end;
			
			T length = end - begin;
			
			sassert(begin != end);
			
			T offset_hole_begin = hole_begin - begin;
			T offset_hole_end = hole_end - begin;
			
			if(offset_hole_begin >= 0 && offset_hole_begin <= length
			&& offset_hole_end >= 0 && offset_hole_end <= length)
			{
				erase_no_overlap(i, hole_begin, hole_end);
				return;
			}
		}
		
		sassert(false);
		throw std::runtime_error("Erase is not within any range");
	}
	
	static bool combine_(T b1, T e1, T b2, T e2, T& retb, T& rete)
	{
		T len1 = e1 - b1;
		T len2 = e2 - b2;
		
		T offs1 = b1 - b2;
		T offs2 = b2 - b1;
		
		T retb, rete;
		
		if(offs1 >= 0 && offs1 < len2)
		{
			rete = std::max(e1 - b1, e2 - b1);
			if(offs2 > 0)
				retb = std::max(offs2, rete);
			else
				retb = 0;
				
			retb += b1;
			rete += b1;
		}
		else if(offs2 >= 0 && offs2 < len1)
		{
			rete = std::max(e1 - b2, e2 - b2);
			if(offs1 > 0)
				retb = std::max(offs1, rete);
			else
				retb = 0;
				
			retb += b2;
			rete += b2;
		}
	}
	
	void insert(T begin, T end)
	{
		
	}
	
	// Amortized O(1)
	// Assumes [hole_begin, hole_end) is a subset of [r->begin, r->end)
	void erase_no_overlap(range_iterator r, T hole_begin, T hole_end)
	{
		T begin = r->begin;
		T end = r->end;
		
		sassert(begin <= hole_begin && end >= hole_end);
			
		if(begin == hole_begin)
		{
			if(end == hole_end)
			{
				// Hole covers the whole range
				erase_(r);
			}
			else
			{
				r->begin = hole_end;
			}
		}
		else if(end == hole_end)
		{
			// begin != hole_begin, so we don't need to check that
			r->end = hole_begin;
		}
		else
		{
			// Hole in the middle.
			// Use the current range for the lower piece and add
			// another for the upper piece.
			r->end = hole_begin;
			ranges.push_back(range(hole_end, end));
		}
		
		return;
	}
	
	range_iterator begin()
	{ return ranges.begin(); }
	
	range_iterator end()
	{ return ranges.end(); }
	
private:
	void extend_forwards_(range_iterator r, T end)
	{
		range_iterator i = r;
		for(; ++i != ranges.end(); )
		{
			if(i->begin == end)
			{
				r->end = i->end;
				erase_(i);
				return;
			}
		}
	}
	
	void extend_backwards_(range_iterator r, T begin)
	{
		range_iterator i = r;
		for(; ++i != ranges.end();)
		{
			if(i->end == begin)
			{
				r->begin = i->begin;
				erase_(i);
				return;
			}
		}
	}
	
	void erase_(range_iterator r)
	{
		*r = ranges.back();
		ranges.pop_back();
	}
	
	std::vector<range> ranges;
};

} // namespace gvl

#endif // UUID_E8CD4134467C409B367DA595A15C7D89

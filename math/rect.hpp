#ifndef UUID_63536246013C464DDB1F129027E54907
#define UUID_63536246013C464DDB1F129027E54907

#include "vec.hpp"

#include <algorithm> // For std::min, std::max

namespace gvl
{

template<typename T>
class basic_rect
{
public:
	basic_rect()
	{ }

	basic_rect(T x1_, T y1_, T x2_, T y2_)
	: x1(x1_), y1(y1_), x2(x2_), y2(y2_)
	{ }
	
	basic_rect(basic_vec<T, 2> const& pixel)
	: x1(pixel.x), y1(pixel.y)
	, x2(pixel.x + T(1)), y2(pixel.y + T(1))
	{ }
	
	basic_rect(basic_vec<T, 2> const& center, T size)
	: x1(center.x - size), y1(center.y - size)
	, x2(center.x + size), y2(center.y + size)
	{ }
	
	basic_rect(basic_vec<T, 2> const& center, T w, T h)
	: x1(center.x - w/2), y1(center.y - h/2)
	, x2(center.x + w/2), y2(center.y + h/2)
	{ }
	
	basic_rect(basic_vec<T, 2> const& corner1, basic_vec<T, 2> const& corner2)
	{
		if(corner1.x < corner2.x)
		{
			x1 = corner1.x;
			x2 = corner2.x + 1;
		}
		else
		{
			x1 = corner2.x;
			x2 = corner1.x + 1;
		}
		
		if(corner1.y < corner2.y)
		{
			y1 = corner1.y;
			y2 = corner2.y + 1;
		}
		else
		{
			y1 = corner2.y;
			y2 = corner1.y + 1;
		}
	}
	
	T x1;
	T y1;
	T x2;
	T y2;
	
	T center_x() const
	{ return (x1 + x2) / T(2); }
	
	T center_y() const
	{ return (y1 + y2) / T(2); }
	
	basic_vec<T, 2> center() const
	{ return basic_vec<T, 2>(center_x(), center_y()); }

	T width() const
	{ return x2 - x1; }

	T height() const
	{ return y2 - y1; }
	
	basic_rect flip() const
	{ return basic_rect<T>(y1, x1, y2, x2); }

	bool valid()
	{ return x1 <= x2 && y1 <= y2; }
	
	void join(basic_rect const& b)
	{
		x1 = std::min(b.x1, x1);
		y1 = std::min(b.y1, y1);
		x2 = std::max(b.x2, x2);
		y2 = std::max(b.y2, y2);
	}

	bool intersecting(basic_rect const& b) const
	{
		return (b.y2 >= y1
		     && b.y1 <= y2
		     && b.x2 >= x1
		     && b.x1 <= x2);
	}

	bool intersect(basic_rect const& b)
	{
		x1 = std::max(b.x1, x1);
		y1 = std::max(b.y1, y1);
		x2 = std::min(b.x2, x2);
		y2 = std::min(b.y2, y2);
		
		return valid();
	}

	bool inside(basic_vec<T, 2> v) const
	{ return inside(v.x, v.y); }
	
	bool inside(T x, T y) const
	{
		T diffX = x - x1;
		T diffY = y - y1;
		
		return diffX < width() && diffX >= T(0)
		    && diffY < height() && diffY >= T(0);
	}
	
	basic_rect operator&(basic_rect const& b) const
	{ basic_rect ret(*this); ret &= b; return ret; }
	
	basic_rect& operator&=(basic_rect const& b)
	{
		intersect(b);
		return *this;
	}
	
	basic_rect operator|(basic_rect const& b) const
	{ basic_rect ret(*this); ret |= b; return ret; }
	
	basic_rect& operator|=(basic_rect const& b)
	{
		join(b);
		return *this;
	}
	
	basic_rect operator+(basic_vec<T, 2> const& b)
	{ basic_rect ret(*this); ret += b; return ret; }
	
	basic_rect& operator+=(basic_vec<T, 2> const& b)
	{
		x1 += b.x; x2 += b.x;
		y1 += b.y; y2 += b.y;
		return *this;
	}
	
	basic_rect operator-(basic_vec<T, 2> const& b)
	{ basic_rect ret(*this); ret -= b; return ret; }
	
	basic_rect& operator-=(basic_vec<T, 2> const& b)
	{
		x1 -= b.x; x2 -= b.x;
		y1 -= b.y; y2 -= b.y;
		return *this;
	}
	
	basic_rect translated(T x, T y)
	{ return *this + basic_vec<T, 2>(x, y); }
	
	void translate_v(T y)
	{
		y1 += y; y2 += y;
	}
	
	void translate_h(T x)
	{
		x1 += x; x2 += x;
	}
	
	basic_vec<T, 2> ul()
	{ return basic_vec<T, 2>(x1, y1); }
	
	basic_vec<T, 2> ur()
	{ return basic_vec<T, 2>(x2, y1); }
	
	basic_vec<T, 2> ll()
	{ return basic_vec<T, 2>(x1, y2); }
	
	basic_vec<T, 2> lr()
	{ return basic_vec<T, 2>(x2, y2); }
};

typedef basic_rect<int> rect;
typedef basic_rect<float> frect;

} // namespace gvl

#endif // UUID_63536246013C464DDB1F129027E54907

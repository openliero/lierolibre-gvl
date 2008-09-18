#ifndef UUID_604BD56BB2F2436A4CC319ACA78603F3
#define UUID_604BD56BB2F2436A4CC319ACA78603F3

#include <cmath>

namespace gvl
{

// Only N == 2 supported at the moment

template<typename T, int N>
struct basic_vec
{
};

template<typename T>
struct basic_vec<T, 2>
{
	typedef T manip_t;
	typedef T coord_type;
	
	basic_vec()
	: x(T(0)), y(T(0))
	{ }
	
	template<class T2>
	explicit basic_vec(basic_vec<T2, 2> const& b)
	: x(static_cast<T>(b.x)), y(static_cast<T>(b.y))
	{ }
	
	/// Vector from a to b.
	///
	basic_vec(basic_vec const& a, basic_vec const& b)
	: x(b.x - a.x), y(b.y - a.y)
	{ }

	basic_vec(T const& x_, T const& y_)
	: x(x_) , y(y_)
	{ }
	
	template<class T2>
	basic_vec(T2 const& x, T2 const& y)
	: x(static_cast<T>(x)), y(static_cast<T>(y))
	{ }
	
	void zero()
	{
		x = T(0); y = T(0);
	}
		
	basic_vec& operator += (basic_vec const& rhs)
	{
		x += rhs.x; y += rhs.y;
		return *this;
	}
	
	basic_vec& operator -= (basic_vec const& rhs)
	{
		x -= rhs.x; y -= rhs.y;
		return *this;
	}
	
	basic_vec& operator *= (T rhs)
	{
		x *= rhs; y *= rhs;
		return *this;
	}
	
	basic_vec& operator /= (T rhs)
	{
		x /= rhs; y /= rhs;
		return *this;
	}
	
	friend basic_vec operator - (basic_vec const& self)
	{
		return basic_vec(-self.x, -self.y);
	}
	
	basic_vec half() const
	{ return basic_vec(x / T(2), y / T(2)); }
		
	T x;
	T y;
	
};

// Operations

template<typename T>
inline basic_vec<T, 2> operator - (basic_vec<T, 2> const& lhs, basic_vec<T, 2> const& rhs)
{ basic_vec<T, 2> ret(lhs); ret -= rhs; return ret; }

template<typename T>
inline basic_vec<T, 2> operator + (basic_vec<T, 2> const& lhs, basic_vec<T, 2> const& rhs)
{ basic_vec<T, 2> ret(lhs); ret += rhs; return ret; }

template<typename T>
inline basic_vec<T, 2> operator * (basic_vec<T, 2> const& lhs, basic_vec<T, 2> const& rhs)
{ basic_vec<T, 2> ret(lhs); ret *= rhs; return ret; }

template<typename T>
inline basic_vec<T, 2> operator * (basic_vec<T, 2> const& lhs, T rhs)
{ basic_vec<T, 2> ret(lhs); ret *= rhs; return ret; }

template<typename T>
inline basic_vec<T, 2> operator * (T lhs, basic_vec<T, 2> const& rhs)
{ basic_vec<T, 2> ret(rhs); ret *= lhs; return ret; }

template<typename T>
inline basic_vec<T, 2> operator / (basic_vec<T, 2> const& lhs, T rhs)
{ basic_vec<T, 2> ret(lhs); ret /= rhs; return ret; }

template<typename T>
inline bool operator==(basic_vec<T, 2> a, basic_vec<T, 2> b)
{ return a.x == b.x && a.y == b.y; }

template<typename T>
inline bool operator!=(basic_vec<T, 2> a, basic_vec<T, 2> b)
{ return !(*this == b); }

template<typename T>
inline basic_vec<T, 2> cross(T a, basic_vec<T, 2> b)
{ basic_vec<T, 2> ret(-a * b.y, a * b.x);  return ret; }

template<typename T>
inline basic_vec<T, 2> cross(basic_vec<T, 2> a, T b)
{ basic_vec<T, 2> ret(b * a.y, -b * a.x);  return ret; }

template<typename T>
inline T cross(basic_vec<T, 2> a, basic_vec<T, 2> b)
{ return a.x * b.y - a.y * b.x; }

template<typename T>
inline T dot(basic_vec<T, 2> a, basic_vec<T, 2> b)
{ return a.x * b.x + a.y * b.y; }

template<typename T>
inline basic_vec<T, 2> perp(basic_vec<T, 2> self)
{ basic_vec<T, 2> ret(-self.y, self.x);  return ret; }

// Rotate a using b
template<typename T>
inline basic_vec<T, 2> rotate(basic_vec<T, 2> a, basic_vec<T, 2> b)
{
	basic_vec<T, 2> ret(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
	return ret;
}

// Unrotate a using b
template<typename T>
inline basic_vec<T, 2> unrotate(basic_vec<T, 2> a, basic_vec<T, 2> b)
{
	basic_vec<T, 2> ret(a.x * b.x + a.y * b.y, a.y * b.x - a.x * b.y);
	return ret;
}

template<typename T>
inline T length_sqr(basic_vec<T, 2> self)
{
	return self.x*self.x + self.y*self.y;
}

template<typename T>
inline double length(basic_vec<T, 2> self)
{ return std::sqrt(double(length_sqr(self))); }

template<typename T>
inline basic_vec<T, 2> normal(basic_vec<T, 2> self)
{
	double invLength = 1.0 / length(self);
	basic_vec<T, 2> ret(T(self.x*invLength), T(self.y*invLength));
	return ret;
}

// Common aliases
typedef basic_vec<float, 2> vec2;
typedef basic_vec<int, 2> ivec2;

} // namespace gvl

#endif // UUID_604BD56BB2F2436A4CC319ACA78603F3

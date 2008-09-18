#ifndef UUID_157EE60961484EB2118DE2BF33EAF464
#define UUID_157EE60961484EB2118DE2BF33EAF464

#include <cmath>
#include "../support/debug.hpp"
#include "vec.hpp"

namespace gvl
{

// Only N == M == 2 supported at the moment

template<typename T, int N, int M, typename D>
struct basic_matrix_common
{
	T v[N * M];
	
	D& operator+=(D const& rhs)
	{
		for(std::size_t i = 0; i < N * M; ++i)
			v[i] += rhs.v[i];
			
		return static_cast<D&>(*this);
	}
};

template<typename T, int N, int M>
struct basic_matrix : basic_matrix_common<T, N, M, basic_matrix<T, N, M> >
{
	
};

template<typename T>
struct basic_matrix<T, 2, 2>
: basic_matrix_common<T, 2, 2, basic_matrix<T, 2, 2> >
{
	T inv_det() const
	{
		return (this->v[0] * this->v[3] - this->v[1] * this->v[2]);
	}
	
	basic_matrix invert() const
	{
		T idet = inv_det();
		sassert(idet != T(0));
		T det = T(1) / idet;
		
		T a = this->v[0], b = this->v[1],
		  c = this->v[2], d = this->v[3];
		
		basic_matrix ret;
		
		ret.v[0] = d *  det;
		ret.v[1] = b * -det;
		ret.v[2] = c * -det;
		ret.v[3] = a *  det;
		
		return ret;
	}
		
	// Solve A * x = b
	basic_vec<T, 2> solve(basic_vec<T, 2> b) const
	{
		T idet = inv_det();
		sassert(idet != T(0));
		T det = T(1) / idet;
		
		basic_vec<T, 2> ret(
			det * (this->v[3] * b.x - this->v[1] * b.y),
			det * (this->v[0] * b.y - this->v[2] * b.x));
			
		return ret;
	}
		
	basic_vec<T, 2> operator*(basic_vec<T, 2> const& rhs) const
	{
		basic_vec<T, 2> ret;
		ret.x = this->v[0] * rhs.x + this->v[1] * rhs.y;
		ret.y = this->v[2] * rhs.x + this->v[3] * rhs.y;
		
		return ret;
	}
};

} // namespace gvl

#endif // UUID_157EE60961484EB2118DE2BF33EAF464

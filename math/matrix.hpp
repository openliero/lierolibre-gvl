#ifndef UUID_157EE60961484EB2118DE2BF33EAF464
#define UUID_157EE60961484EB2118DE2BF33EAF464

#include <cmath>
#include "../support/debug.hpp"
#include "vec.hpp"

namespace gvl
{

// Only N == M == 2 supported at the moment

// N = columns
// M = rows
template<typename T, int N, int M, typename D>
struct basic_matrix_common
{
	T v[N * M];

	T& operator()(int row, int column)
	{
		return v[row*N + column];
	}

	D& operator+=(D const& rhs)
	{
		for(std::size_t i = 0; i < N * M; ++i)
			v[i] += rhs.v[i];

		return static_cast<D&>(*this);
	}

	D& operator*=(T rhs)
	{
		for(std::size_t i = 0; i < N * M; ++i)
			v[i] *= rhs;

		return static_cast<D&>(*this);
	}

	static D identity()
	{
		D ret;
		for(std::size_t i = 0; i < N * M; ++i)
			ret.v[i] = T(0);
		for(std::size_t i = 0; i < (N < M ? N : M); ++i)
			ret.v[i + i*N] = T(1);
		return ret;
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
	T det() const
	{
		return (this->v[0] * this->v[3] - this->v[1] * this->v[2]);
	}

	basic_matrix invert() const
	{
		T determinant = det();
		sassert(determinant != T(0));
		T idet = T(1) / determinant;

		T a = this->v[0], b = this->v[1],
		  c = this->v[2], d = this->v[3];

		basic_matrix ret;

		ret.v[0] = d *  idet;
		ret.v[1] = b * -idet;
		ret.v[2] = c * -idet;
		ret.v[3] = a *  idet;

		return ret;
	}

	// Solve A * x = b
	basic_vec<T, 2> solve(basic_vec<T, 2> b) const
	{
		T determinant = inv_det();
		sassert(determinant != T(0));
		T idet = T(1) / determinant;

		basic_vec<T, 2> ret(
			idet * (this->v[3] * b.x - this->v[1] * b.y),
			idet * (this->v[0] * b.y - this->v[2] * b.x));

		return ret;
	}

	/// Initialize this matrix using an angle. This matrix becomes
	/// an orthonormal rotation matrix.
	void assign_rotation(T angle)
	{
		// To enable overloading
		using std::cos;
		using std::sin;

		T c = cos(angle), s = sin(angle);
		ret.v[0] = c;
		ret.v[1] = -s;
		ret.v[2] = s;
		ret.v[3] = c;

		/*
		col1.x = c; col2.x = -s;
		col1.y = s; col2.y = c;
		*/
	}

	basic_vec<T, 2> operator*(basic_vec<T, 2> const& rhs) const
	{
		basic_vec<T, 2> ret;
		ret.x = this->v[0] * rhs.x + this->v[1] * rhs.y;
		ret.y = this->v[2] * rhs.x + this->v[3] * rhs.y;

		return ret;
	}
};

typedef basic_matrix<float, 2, 2> fmat2x2;

} // namespace gvl

#endif // UUID_157EE60961484EB2118DE2BF33EAF464

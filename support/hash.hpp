#ifndef UUID_5849ECCC7F1142CA9E5E9CA33B298A6D
#define UUID_5849ECCC7F1142CA9E5E9CA33B298A6D

#include <cstddef>
#include <utility>

namespace gvl
{

inline std::size_t hash(std::size_t v);

inline std::size_t hash(int v)
{
	return hash(std::size_t(v));
}

inline std::size_t hash(std::size_t v)
{
	return v * 2654435761ul ^ (v >> 16);
}

template<typename T1, typename T2>
inline std::size_t hash(std::pair<T1, T2> const& v)
{
	return (hash(v.first) * 2654435761ul) ^ hash(v.second);
}

template<typename T>
inline std::size_t hash(T const& v)
{
	return v.hash();
}

struct hash_functor
{
	template<typename T>
	std::size_t operator()(T const& v) const
	{
		return hash(v);
	}
};

std::size_t next_prime(std::size_t n);
std::size_t prev_prime(std::size_t n);

} // namespace vl

#endif // UUID_5849ECCC7F1142CA9E5E9CA33B298A6D

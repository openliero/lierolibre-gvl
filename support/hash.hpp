#ifndef UUID_5849ECCC7F1142CA9E5E9CA33B298A6D
#define UUID_5849ECCC7F1142CA9E5E9CA33B298A6D

#include <cstddef>
#include <utility>
#include <string>
#include "cstdint.hpp"

namespace gvl
{

typedef uint32_t hash_t;
int const hash_bits = 32;

inline hash_t hash(uint32_t v, uint32_t p = 0x3C618459)
{
	v ^= p;
	v *= v * 2 + 1;
	return p - v;
}

inline hash_t hash(int v, uint32_t p = 0x3C618459)
{ return hash(uint32_t(v), p); }

inline hash_t hash(void const* v, uint32_t p = 0x3C618459)
{ return hash(hash_t(std::size_t(v)), p); }

template<typename T1, typename T2>
inline hash_t hash(std::pair<T1, T2> const& v)
{
	return (hash(v.first) * 2654435761ul) ^ hash(v.second);
}

inline hash_t hash(std::string const& v, unsigned p = 0x3C618459)
{
	std::size_t amount = 512;
	
	if(amount > v.size())
		amount = v.size();
		
	hash_t h = p;
	for(std::size_t i = 0; i < amount; ++i)
	{
		h = h*33 ^ (unsigned char)v[i];
	}
	return h;
}

template<typename T>
inline hash_t hash(T const& v, unsigned p = 0x3C618459)
{
	return v.hash(p);
}

struct hash_functor
{
	template<typename T>
	hash_t operator()(T const& v, unsigned p = 0x3C618459) const
	{
		return hash(v, p);
	}
};

std::size_t next_prime(std::size_t n);
std::size_t prev_prime(std::size_t n);

} // namespace vl

#endif // UUID_5849ECCC7F1142CA9E5E9CA33B298A6D

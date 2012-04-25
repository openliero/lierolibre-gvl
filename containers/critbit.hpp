#ifndef UUID_DCF3012792AF4ABD1F44ED84EABEA581
#define UUID_DCF3012792AF4ABD1F44ED84EABEA581

#include "../support/cstdint.hpp"
#include <cstddef>

namespace gvl
{

struct critbit
{
	void* root;

	bool contains(uint8_t const* u, std::size_t ulen);
	int insert(uint8_t const* u, std::size_t ulen);
};



}

#endif // UUID_DCF3012792AF4ABD1F44ED84EABEA581

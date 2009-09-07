#ifndef UUID_B28D1ACCA789486008A1FF8B92CF00C5
#define UUID_B28D1ACCA789486008A1FF8B92CF00C5

#include <stdexcept>

namespace gvl
{

struct assert_failure : std::runtime_error
{
	assert_failure(std::string const& str)
	: std::runtime_error(str)
	{
	}
};

void passert_fail(char const* cond, char const* file, int line, char const* msg);

#ifndef NDEBUG
#define passert(cond, msg) \
	if(!(cond)) gvl::passert_fail(#cond, __FILE__, __LINE__, msg)
#define sassert(cond) \
	if(!(cond)) gvl::passert_fail(#cond, __FILE__, __LINE__, "")
#else
#define passert(cond, msg) ((void)0)
#define sassert(cond) ((void)0)
#endif

#define GVL_STATIC_ASSERT(cond) typedef char static_assert_[(cond) ? 1 : 0]

}

#endif // UUID_B28D1ACCA789486008A1FF8B92CF00C5

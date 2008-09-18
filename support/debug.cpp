#include "debug.hpp"

//#include <exception>
#include <cstdio>
#include <cstdlib>

namespace gvl
{

/*
struct assert_failure : public std::exception
{
	assert_failure(char const* str)
	: str(str)
	{
	}
	
	char const* what() throw()
	{
		return str;
	}
	
	char const* str;
};

void fail_assert(char const* str)
{
	throw assert_failure(str);
}
*/

void passert_fail(char const* cond, char const* file, int line, char const* msg)
{
	std::fprintf(stderr, "PASSERT FAILED: %s:%d: !(%s)  %s\n", file, line, cond, msg);
	std::fflush(stderr); /* Make sure it's written */
	std::abort();
}

}

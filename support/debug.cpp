#include "debug.hpp"

#include <string>

void gvl::passert_fail(char const* cond, char const* file, int line, char const* msg)
{
	std::string s;
	
	s += "ASSERT FAILED: ";
	s += file;
	s += ":";
	s += line;
	s += ": !(";
	s += cond;
	s += "), ";
	s += msg;

	throw gvl::assert_failure(s);
}

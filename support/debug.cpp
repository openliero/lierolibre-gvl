#include "debug.hpp"


#include <cstdio>
#include <cstdlib>
#include <sstream>

namespace gvl
{

void passert_fail(char const* cond, char const* file, int line, char const* msg)
{
	std::ostringstream ss;
	
	ss << "ASSERT FAILED: " << file << ":" << line << ": !(" << cond << "), " << msg << std::endl;
	 
	throw assert_failure(ss.str());
}

}

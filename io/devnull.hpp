#ifndef UUID_1C533FF78AD74C33BFB68ABED75A692E
#define UUID_1C533FF78AD74C33BFB68ABED75A692E

#include "stream.hpp"

namespace gvl
{

struct devnull : stream
{
	read_result read(size_type amount = 0, bucket* dest = 0)
	{
		return read_result(read_eos);
	}
	
	write_result write(bucket* b)
	{
		unlink(b);
		delete b;
		return write_result(write_ok, true);
	}
};

}

#endif // UUID_1C533FF78AD74C33BFB68ABED75A692E

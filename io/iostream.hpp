#ifndef GVL_IOSTREAM_HPP
#define GVL_IOSTREAM_HPP

#include "encoding.hpp"

namespace gvl
{

inline octet_stream_writer& cout()
{
	static octet_stream_writer sr(new fstream(stdout));
	return sr;
}

}

#endif // GVL_IOSTREAM_HPP

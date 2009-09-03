#ifndef UUID_ADEA58A2C81F42C25E8CAFA32ED72A18
#define UUID_ADEA58A2C81F42C25E8CAFA32ED72A18

#include "stream.hpp"
#include "convert.hpp"

namespace gvl
{

template<typename Reader = stream_reader, typename Writer = stream_writer>
struct formatter
{
	formatter(Reader& r_init, Writer& w_init)
	: r(r_init)
	, w(w_init)
	{
	}
		
	Reader& r;
	Writer& w;
};

template<typename R, typename W>
inline formatter<R, W>& operator<<(formatter<R, W>& self, uint32_t x)
{
	uint_to_ascii_base<10>(self.w, x);
	return self;
}

}

#endif // UUID_ADEA58A2C81F42C25E8CAFA32ED72A18

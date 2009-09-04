#ifndef UUID_ADEA58A2C81F42C25E8CAFA32ED72A18
#define UUID_ADEA58A2C81F42C25E8CAFA32ED72A18

#include "stream.hpp"
#include "convert.hpp"
#include <cstring>

namespace gvl
{

template<typename Reader = stream_reader, typename Writer = stream_writer>
struct format_writer
{
	format_writer(Writer& w_init)
	: w(w_init)
	{
	}
	
	void flush()
	{ w.flush(); }
		
	Writer& w;
};

template<typename R, typename W>
inline format_writer<R, W>& operator<<(format_writer<R, W>& self, uint32_t x)
{
	uint_to_ascii_base<10>(self.w, x);
	return self;
}

template<typename R, typename W>
inline format_writer<R, W>& operator<<(format_writer<R, W>& self, int32_t x)
{
	int_to_ascii_base<10>(self.w, x);
	return self;
}

template<typename R, typename W>
inline format_writer<R, W>& operator<<(format_writer<R, W>& self, char const* str)
{
	self.w.put(reinterpret_cast<uint8_t const*>(str), std::strlen(str));
	return self;
}

template<typename R, typename W>
inline format_writer<R, W>& operator<<(format_writer<R, W>& self, char ch)
{
	self.w.put(static_cast<uint8_t>(ch));
	return self;
}

}

#endif // UUID_ADEA58A2C81F42C25E8CAFA32ED72A18

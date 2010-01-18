#ifndef UUID_0DA946DFD27148AC249303959F48AA8C
#define UUID_0DA946DFD27148AC249303959F48AA8C

#include "encoding.hpp"

namespace gvl
{

// NOTE: This does not yet handle surrogate pairs, so it only works with characters in BMP.

struct utf16_be_stream_reader : protected octet_stream_reader
{
	typedef octet_stream_reader base;
	
	utf16_be_stream_reader(stream_ptr str)
	: base(str)
	{
	}
	
	uint32_t get()
	{
		uint8_t h = base::get();
		uint8_t l = base::get();
		
		// TODO: Handle surrogate pairs
		
		return (h << 8) | l;
	}
	
	void get(uint32_t* dest, std::size_t len)
	{
		// TODO: Can optimize this
		for(std::size_t i = 0; i < len; ++i)
			dest[i] = get();
	}
};

} // namespace gvl

#endif // UUID_0DA946DFD27148AC249303959F48AA8C

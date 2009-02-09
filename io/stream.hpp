#ifndef UUID_745F17B8515E4ED264BDDBBBBED3E8B4
#define UUID_745F17B8515E4ED264BDDBBBBED3E8B4

#include <stdexcept>
#include <cstring>
#include <string>
#include <memory>
#include "../support/debug.hpp"
#include "../resman/shared_ptr.hpp"
#include "brigade.hpp"

namespace gvl
{

struct istream
: bucket_reader<istream>
{
	istream()
	: bucket_reader<stream>(&source_brigade)
	{
	}
	brigade source_brigade;
};

struct ostream
: bucket_writer
{
	std::auto_ptr<bucket_sink> sink;
	brigade sink_brigade;
};

struct stream
: bucket_reader<stream>
, bucket_writer
, bucket_sink
{
	stream()
	: bucket_reader<stream>(&source_brigade)
	, bucket_writer(&sink_brigade)
	{
	}
	
	stream(bucket* head)
	: bucket_reader<stream>(&source_brigade)
	, bucket_writer(&sink_brigade)
	{
		source_brigade.append(head);
	}
	
	void open(bucket* source)
	{
		sassert(source_brigade.buckets.empty());
		source_brigade.append(source);
	}
		
#if 0
	void blocking(void (*blocking_new)(device_buf*))
	{
		passert(get(), "No device");
		get()->blocking(blocking_new);
	}

	void exceptions(unsigned char f)
	{
		passert(dev, "No device");
		get()->exceptions(f);
	}

	operator void*()
	{ return (get() && get()->good()) ? get() : 0; }
	
	void close()
	{ if(get()) get()->close(); }
#endif
	
	virtual ~stream()
	{
#if 0
		close();
#endif
	}
	
	// Input
#if 0
	std::size_t in_size()
	{ return get()->in_size(); }
	
	// Tries to buffer //size// bytes
	bool in_read(std::size_t size)
	{ return get()->in_read(size); }
		
	bool eof()
	{ return get()->eof(); }

	
	void in_pad(std::size_t size)
	{ return get()->in_pad(size); }
	
	// Returns a pointer to a buffer read_size() long
	byte const* in_buffer() const
	{ return get()->in_buffer(); }
	
	// Consumes the first //size// bytes in the buffer.
	// All consumption must go through this function.
	void in_consume(std::size_t size)
	{ get()->in_consume(size); }

	bool in_aint(unsigned int& v);
	bool in_int32(unsigned int& dest);
	bool in_int24(unsigned int& dest);
	bool in_int16(unsigned int& dest);
	bool in_int8(unsigned int& dest);
	bool in_sint16(int& dest);
	bool in_sint32(int& dest);
	bool in_string16(std::string& dest);
	
	// Output
	
	void out(byte const* p, std::size_t size)
	{ get()->out(p, size); }
	
	void out(buffer const& buf)
	{ get()->out(buf.data(), buf.size()); }
	
	byte* out_reserve(std::size_t size)
	{ return dev->out_reserve(size); }
	
	bool flush()
	{
		if(dev)
			return dev->flush();
		return true;
	}
	
	bool blocking_flush()
	{
		if(dev)
		{
			while(!dev->flush() && *this)
			{ dev->blocking(dev); }
			
			return dev->out_buf.empty();
		}
		
		return true;
	}
	
	std::size_t unflushed_data()
	{ return dev->out_buf.size(); }
#endif

	/// Add another source bucket
	void add_source(bucket* b)
	{ source_brigade.append(b); }
	
	bucket_sink::status flush()
	{
		bucket_writer::flush();

		while(!sink_brigade.buckets.empty())
		{
			bucket_sink::status ret = write(sink_brigade.buckets.last());
			if(ret != bucket_sink::ok)
				return ret;
		}
		
		return bucket_sink::ok;
	}

	/// For bucket_reader<stream>
	void block()
	{ throw "can't block"; }

protected:
	// Non-copyable
	stream(stream const&);
	stream& operator=(stream const&);
	
	brigade source_brigade;
	brigade sink_brigade;
	//std::auto_ptr<bucket_sink> sink;
};

#if 0

void out_aint(stream& s, unsigned int v)
{
	while(true)
	{
		int b = v & 0x7f;
		v >>= 7;
		
		if(v != 0)
			out_int8(s, b | 0x80);
		else
		{
			out_int8(s, b);
			break;
		}
	}
}

void out_int32(unsigned int v)
{
	byte* dest = out_reserve(4);
	dest[0] = v & 0xFF;
	dest[1] = (v >> 8) & 0xFF;
	dest[2] = (v >> 16) & 0xFF;
	dest[3] = (v >> 24) & 0xFF;
}

void out_int24(unsigned int v)
{
	byte* dest = out_reserve(3);
	dest[0] = v & 0xFF;
	dest[1] = (v >> 8) & 0xFF;
	dest[2] = (v >> 16) & 0xFF;
}

void out_int16(unsigned int v)
{
	byte* dest = out_reserve(2);
	dest[0] = v & 0xFF;
	dest[1] = (v >> 8) & 0xFF;
}

void out_int8(unsigned int v)
{
	byte* dest = out_reserve(1);
	dest[0] = v & 0xFF;
}

void out_sint16(int v)
{
	out_int16((unsigned int)(v + 0x8000));
}

void out_sint32(int v)
{
	out_int32((unsigned int)(v + 0x80000000));
}

void out_string16(std::string const& src)
{
	out_int16(int(src.size()));
	out((byte const*)src.data(), src.size());
}


bool extract_line(device_buf* src, std::string& str);

inline bool extract_line(istream& src, std::string& str)
{
	return extract_line(src.get_devbuf(), str);
}

#endif

} // namespace gvl

#endif // UUID_745F17B8515E4ED264BDDBBBBED3E8B4

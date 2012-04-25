#include "device_buf.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include "../support/debug.hpp"
#include "../support/log.hpp"

namespace gvl
{

byte* device_buf::out_reserve(std::size_t size)
{
	return out_buf.insert_uninitialized(size);
}

// Writes the bytes in [p, p + size) to the stream
void device_buf::out(byte const* p, std::size_t size)
{
	if(out_buf.empty() && size > out_buffer_size)
	{
		// Try to bypass buffering to avoid unnecessary copying of larger blocks
		try
		{
			std::size_t written = write(p, size);
			sassert(written <= size);

			size -= written;
			if(size == 0)
				return; // We wrote all of it successfully, nothing to buffer so return
			p += written;
		}
		catch(stream_error&)
		{
			state.set(error_bit);
			throw;
		}
	}

	// Insert into stream
	out_buf.insert(p, size);
	if(out_buf.size() > out_buffer_size)
		flush(); // Time to flush
}

// Tries to write the buffered data to the stream
bool device_buf::flush()
{
	if(out_buf.empty())
		return true;

	try
	{
		std::size_t written = write(&out_buf[0], out_buf.size());

		sassert(written <= out_buf.size());
		out_buf.consume(written);

		out_buffer_size = std::max(std::size_t(1024ul), out_buf.size() * std::size_t(2ul)); // TODO: What factor should be used here?
	}
	catch(stream_error&)
	{
		state.set(error_bit);
		throw;
	}

	return out_buf.empty();

}

void device_buf::do_close()
{

}

void device_buf::close()
{
	flush();
	do_close();
}

/////

device_buf::device_buf()
: out_buffer_size(1024), in_blocking(0)
, state(0)
, throw_on(0)
{
}

device_buf::device_buf(byte const* p, std::size_t s)
: in_buf(p, s), in_blocking(0)
, state(0)
, throw_on(0)
{
}

device_buf::~device_buf()
{
	//close(); // We can't close here because it might make us call a pure-virtual function
}


// Tries to buffer //size// bytes. Returns true if successful, false otherwise.
// This function may block until the request is fulfilled if a blocking policy is specified.
bool device_buf::in(std::size_t size)
{
	if(in_available() >= size)
		return true;

	try
	{
		while(true)
		{
			try_read_amount_(size);

			if(in_available() >= size)
				return true;

			if(in_blocking)
			{
				flush(); // Perhaps we don't get any data because some request is still in the pipeline, flush
				in_blocking(this);
				continue;
			}
			return false;
		}
	}
	// TODO: Optional throw on eof
	catch(stream_eof&)
	{
		if(throw_on.any(eof_bit))
			throw stream_error(0);
		return false;
	}
	catch(stream_error&)
	{
		state.set(error_bit);
		throw;
	}
}

bool device_buf::eof() const
{
	return state.any(eof_bit);
}

// Tries to buffer //size// bytes. Returns true if any bytes were buffered.
bool device_buf::try_read_amount_(std::size_t size)
{
	passert(size >= in_buf.size(), "Request met already");

	std::size_t old_size = in_buf.size();

	std::size_t add = size - old_size;

	if(add < in_buffer_size)
	{
		add = in_buffer_size; // We try to read at least buffer_size bytes
		size = old_size + add;
	}

	in_buf.resize(size);

	sassert(size == in_buf.size());
	sassert(size == old_size + add);


	try
	{
		std::size_t amount_read = read(&in_buf[old_size], add);
		in_buf.resize(old_size + amount_read);
		return amount_read > 0;
	}
	catch(stream_eof&)
	{
		in_buf.resize(old_size);
		state.set(eof_bit);
		throw;
	}

	return false;
}

void device_buf::in_pad(std::size_t size)
{
	in_buf.insert(byte(0), size);
}

void device_buf::in_direct_fill(byte const* p, std::size_t size)
{
	in_buf.insert(p, size);
}

// Returns a pointer to a buffer in_available() long
byte const* device_buf::in_buffer() const
{
	return in_buf.data();
}

// Consumes the first //size// bytes in the buffer
void device_buf::in_consume(std::size_t size)
{
	in_buf.consume(size);
}

} // namespace gvl

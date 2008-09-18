#include "stream.hpp"

#if 0

bool stream::in_aint(unsigned int& v)
{
	read(5);
	
	std::size_t max = in_size();
	byte const* p = in_buffer();
	
	v = 0;
	
	for(std::size_t i = 0; i < max; ++i)
	{
		byte b = p[i];
		
		if((b & 0x80) == 0)
		{
			v |= b;
			return true;
		}
		else
		{
			v = (v << 7) | (b & 0x7f);
		}
	}
	
	return false;
}

bool stream::in_int32(unsigned int& dest)
{
	if(!in(4))
		return false;
	byte const* buf = in_buffer();
	dest = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	in_consume(4);
	return true;
}

bool stream::in_int24(unsigned int& dest)
{
	if(!in(3))
		return false;
	byte const* buf = in_buffer();
	dest = buf[0] | (buf[1] << 8) | (buf[2] << 16);
	in_consume(3);
	return true;
}

bool stream::in_int16(unsigned int& dest)
{
	if(!in(2))
		return false;
	byte const* buf = in_buffer();
	dest = buf[0] | (buf[1] << 8);
	in_consume(2);
	return true;
}

bool stream::in_int8(unsigned int& dest)
{
	if(!in(1))
		return false;
	byte const* buf = in_buffer();
	dest = buf[0];
	in_consume(1);
	return true;
}

bool stream::in_sint16(int& dest)
{
	unsigned int v;
	if(!in_int16(v))
		return false;
	dest = v - 0x8000;
	return true;
}

bool stream::in_sint32(int& dest)
{
	unsigned int v;
	if(!in_int32(v))
		return false;
	dest = v - 0x80000000;
	return true;
}

bool stream::in_string16(std::string& dest)
{
	unsigned int len;
	if(!in_int16(len)
	|| !in(len))
		return false;
		
	dest.assign((char const*)in_buffer(), len);
	in_consume(len);
	return true;
}

#endif

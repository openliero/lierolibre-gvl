#include "bitstream.hpp"
#include "../support/bits.hpp"
#include "../support/debug.hpp"
#include <cstdio>

namespace gvl
{

template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put(uint32_t bit)
{
	uint32_t left = out_bits_left - 1;

	unsigned int bits = out_bits | (bit << left);
	if(left == 0)
	{
		derived().put_byte(word(bits));
		out_bits = 0;
		out_bits_left = word_bits;
	}
	else
	{
		if(out_bits_left == word_bits)
			allocate_out_byte(); // Just began on this byte, allocate it
		out_bits_left = left;
		out_bits = bits;
	}
}

template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put_uint(uint32_t i, uint32_t bits)
{
	// Bits must be at most 32
	sassert(bits <= 32);
	
	if(bits == 0) // Need this special case, as lsb_mask is not defined for 0
		return;
		
	/* TODO: Reserve memory first and use unchecked put byte */
	
	i &= gvl::lsb_mask(bits); /* Clear bits that won't get written */
	if(bits > out_bits_left)
	{
		bits -= out_bits_left; /* bits is now the amount to right shift (bits left to write afterwards) */
		/* bits will be in [1, 31] as out_bits_left is at least 1 */
		out_bits |= i >> bits;
		derived().put_byte(word(out_bits));
		if(bits > 0)
			derived().allocate_out_byte();
			
		for(; bits >= word_bits; )
		{
			bits -= word_bits; /* bits is now the amount to right shift (bits left to write afterwards) */
			derived().put_byte(word(i >> bits));
			if(bits > 0)
				derived().allocate_out_byte();
		}
		
		/* We know bits < 8, so we can just write in the rest into out_bits */
		out_bits = (i << (word_bits - bits));
		out_bits_left = word_bits - bits;
	}
	else if(bits == out_bits_left)
	{
		/* Fitting exactly */
		out_bits |= i; /* No need to shift in place because it is already */
		derived().put_byte(word(out_bits));
		out_bits = 0;
		out_bits_left = word_bits;
	}
	else
	{
		/* We can fit it in */
		out_bits_left -= bits; /* out_bits_left is now the amount to left shift */
		/* out_bits_left is the number of zeroes
		** after the last written bit. */
		out_bits |= i << out_bits_left;
	}
}

template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put_block(void const* ptr_, size_t len)
{
	uint8_t const* ptr = static_cast<uint8_t const*>(ptr_);
	uint8_t const* end;
	if(len == 0)
		return;
	end = ptr + len;

	/* TODO: Reserve memory first and use unchecked put byte */
	
	if(out_bits_left == word_bits)
	{
		/* Just write
		** TODO: memcpy */
		do
		{
			derived().allocate_out_byte();
			derived().put_byte(*ptr);
		}
		while(++ptr != end);
		/* out_bits should be 0 already, since out_bits_left was 8 */
	}
	else
	{
		/* We need to shift */
		
		unsigned int v = out_bits;
		
		unsigned int left = out_bits_left;
		unsigned int right = word_bits - left;
		
		do
		{
			unsigned int b = *ptr++;
			v += (b >> right);
			derived().put_byte(word(v));
			derived().allocate_out_byte();
			v = (b << left);
		}
		while(ptr != end);
		
		out_bits = v;
		/* out_bits_left is correct since we've only written whole bytes */
	}
}

/* Obsoleted in favour of put_trunc
void bitstream::put_lim(uint32_t v, uint32_t low, uint32_t high)
{
	passert(v >= low && v <= high, "v is out of range");
	while(low != high)
	{
		unsigned long mid = low + (high - low)/2;
		if(v > mid)
		{
			put(1);
			low = mid+1;
		}
		else
		{
			put(0);
			high = mid;
		}
	}
}
*/

#if 1
// NOTE: Untested
// Integer in [0, count)
template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put_trunc(uint32_t v, uint32_t count)
{
	uint32_t bits = gvl::ceil_log2(count);
	uint32_t p2 = (1 << bits);
	uint32_t b = count - p2;
	if(v < p2 - b)
		put_uint(v, bits);
	else
		put_uint(v + b, bits + 1);
}

// NOTE: Untested
template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put_unary(uint32_t v)
{
	for(; v >= 32; v -= 32)
		put_uint(0, 32);
	put_uint(1, v+1);
}

// NOTE: Untested
template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put_golomb(uint32_t v, uint32_t m)
{
	uint32_t quot = v / m;
	uint32_t rem = v % m;
	
	put_unary(quot);
	put_trunc(rem, m);
}

// NOTE: Untested
template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put_rice(uint32_t v, uint32_t shift)
{
	sassert(shift > 0);
	uint32_t quot = v >> shift;
	uint32_t rem = v & gvl::lsb_mask(shift);
	
	put_unary(quot);
	put_uint(rem, shift);
}
#endif

template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::put_debug_mark()
{
	put_uint((1<<15)|1, 16);
}


template<typename DerivedT, typename WordT>
void basic_ibitstream<DerivedT, WordT>::ignore(uint32_t bits)
{
	uint32_t cursor = word_bits - in_bits_left;
	cursor += bits;
	
	uint32_t bytes_to_read = cursor / word_bits;
	
	if(bytes_to_read >= 1)
	{
		derived().ignore_bytes(bytes_to_read - 1);
		in_bits = derived().get_byte();
	}
	
	in_bits_left = word_bits - (cursor % word_bits);
}

template<typename DerivedT, typename WordT>
uint32_t basic_ibitstream<DerivedT, WordT>::get()
{
	if(in_bits_left == 0)
	{
		in_bits = derived().get_byte();
		in_bits_left = word_bits;
	}
	
	return (in_bits >> --in_bits_left) & 1;
}


template<typename DerivedT, typename WordT>
uint32_t basic_ibitstream<DerivedT, WordT>::get_uint(uint32_t bits)
{
	// Bits must be at most 32
	sassert(bits <= 32);
	
	if(bits == 0) // Need this special case, as lsb_mask and shl_1_32 are not defined for 0
		return 0;
	uint32_t orig_bits = bits;
	uint32_t v;
	
	/* NOTE: in_bits_left can be 0 */
	
	if(bits > in_bits_left)
	{
		bits -= in_bits_left; /* bits is now the amount to left shift (bits left to read afterwards) */
		/* If in_bits_left is 0 and orig_bits is 32, then bits
		** will be 32 at this point. The normal << is not guaranteed to work as intended. */
		//v = gvl::shl_1_32(in_bits, bits);
		v = in_bits;
		
		for(; bits >= word_bits; )
		{
			bits -= word_bits; /* bits is now the amount to left shift (bits left to read afterwards) */
			v = (v << word_bits) | derived().get_byte();
		}
		
		if(bits > 0)
		{
			in_bits_left = word_bits - bits;
			in_bits = derived().get_byte();
			v = (v << bits) | (in_bits >> in_bits_left);
		}
		else
			in_bits_left = 0;
	}
	else if(bits == in_bits_left)
	{
		/* Exactly what we have */
		v = in_bits;
		in_bits_left = 0;
	}
	else
	{
		/* There's enough */
		in_bits_left -= bits;
		/* in_bits_left is the number of uninteresting bits
		** after the last wanted. */
		v = (in_bits >> in_bits_left);
	}
	
	return v & gvl::lsb_mask(orig_bits);
}

template<typename DerivedT, typename WordT>
void basic_ibitstream<DerivedT, WordT>::get_block(void* ptr_, size_t len)
{
	if(len == 0)
		return;
		
	uint8_t* ptr = static_cast<uint8_t*>(ptr_);	
	uint8_t* end = ptr + len;
	
	if(in_bits_left == 0)
	{
		/* Just write
		** TODO: Check that there is enough to read and memcpy */
		do
			*ptr = derived().get_byte();
		while(++ptr != end);
		/* in_bits will be overwritten later */
	}
	else
	{
		/* We need to shift */

		unsigned int right = in_bits_left;
		unsigned int left = word_bits - right;
		unsigned int v = in_bits << left;
		
		unsigned int b;
		do
		{
			b = derived().get_byte();
			v += (b >> right);
			*ptr++ = word(v);
			v = (b << left);
		}
		while(ptr != end);
		
		in_bits = word(b);
		/* in_bits_left is correct since we've only written whole bytes */
	}
}

// NOTE: TODO: These functions can't handle empty ranges.
/* Obsoleted in favour of get_trunc
uint32_t bitstream::get_lim(uint32_t low, uint32_t high)
{
	while(low != high)
	{
		uint32_t mid = low + (high - low)/2;
		if(get())
			low = mid+1;
		else
			high = mid;
	}
	return low;
}
*/

#if 1
template<typename DerivedT, typename WordT>
uint32_t basic_ibitstream<DerivedT, WordT>::get_trunc(uint32_t count)
{
	uint32_t bits = gvl::ceil_log2(count);
	uint32_t p2 = (1 << bits);
	uint32_t b = count - p2;
	uint32_t v = get_uint(bits);
	if(v < p2 - b)
		return v;
	else
		return ((v << 1) + get()) - b;
}

// NOTE: Untested
template<typename DerivedT, typename WordT>
uint32_t basic_ibitstream<DerivedT, WordT>::get_unary()
{
	uint32_t v = 0;
	for(; !get(); ++v)
		/* Nothing */;
	return v;
}

// NOTE: Untested
template<typename DerivedT, typename WordT>
uint32_t basic_ibitstream<DerivedT, WordT>::get_golomb(uint32_t m)
{
	uint32_t quot = get_unary();
	uint32_t rem = get_trunc(m);
	
	return quot * m + rem;
}

// NOTE: Untested
template<typename DerivedT, typename WordT>
uint32_t basic_ibitstream<DerivedT, WordT>::get_rice(uint32_t shift)
{
	sassert(shift > 0);
	
	uint32_t quot = get_unary();
	uint32_t rem = get_uint(shift);
	
	return (quot << shift) + rem;
}

#endif

template<typename DerivedT, typename WordT>
void basic_ibitstream<DerivedT, WordT>::get_debug_mark()
{
	uint32_t m = get_uint(16);
	passert(m == ((1<<15)|1), "Debug mark not found");
}

template<typename DerivedT, typename WordT>
void basic_ibitstream<DerivedT, WordT>::resetg()
{
	in_bits_left = 0;
	in_bits = 0;
}

template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::resetp()
{
	out_bits_left = 8;
	out_bits = 0;
}

/*
template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::clear()
{
	resetp();
	derived().clear();
}

template<typename DerivedT, typename WordT>
void basic_ibitstream<DerivedT, WordT>::clear()
{
	resetg();
	derived().clear();
}*/

template<typename DerivedT, typename WordT>
void basic_obitstream<DerivedT, WordT>::finish()
{
	if(out_bits_left < word_bits)
	{
		derived().put_byte(word(out_bits));
		out_bits = 0;
		out_bits_left = word_bits;
	}
}

} // namespace gvl

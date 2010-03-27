#include "bitstream.hpp"
#include "../support/bits.hpp"
#include "../support/debug.hpp"
#include <cstdio>

namespace gvl
{

template<typename DerivedT>
void basic_obitstream<DerivedT>::put(uint32_t bit)
{
	uint32_t left = out_bits_left - 1;

	unsigned int bits = out_bits | (bit << left);
	if(left == 0)
	{
		derived().put_byte(byte(bits));
		out_bits = 0;
		out_bits_left = 8;
	}
	else
	{
		if(out_bits_left == 8)
			allocate_out_byte(); // Just began on this byte, allocate it
		out_bits_left = left;
		out_bits = bits;
	}
}


template<typename DerivedT>
void basic_obitstream<DerivedT>::put_uint2(uint32_t i, uint32_t bits)
{
	// Bits must be at most 32
	sassert(bits <= 32);
	
	if(bits == 0) // Need this special case, as lsb_mask is not defined for 0
		return;

	i &= gvl::lsb_mask(bits); /* Clear bits that won't get written */

	uint32_t local_out_bits_left = out_bits_left;
	bitbuf_t local_out_bits = out_bits;

	if(local_out_bits_left > bits)
	{
		/* We can fit it in */
		out_bits = (local_out_bits << bits) | i;
		out_bits_left = local_out_bits_left - bits;
	}
	else
	{
		// local_out_bits_left <= bits
		// local_out_bits_left >= 1
		int left_over = bits - local_out_bits_left;

		// local_out_bits_left may be 32 here, but in those cases local_out_bits == 0.
		local_out_bits = (local_out_bits << local_out_bits_left) | (i >> left_over);

		while(true)
		{
			derived().put_byte(byte(local_out_bits >> 24));
			derived().put_byte(byte(local_out_bits >> 16));
			derived().put_byte(byte(local_out_bits >> 8));
			derived().put_byte(byte(local_out_bits));

			int const bufbits = 32;

			if(bufbits >= 32 || left_over < bufbits)
				break;

			left_over -= bufbits;
			local_out_bits = (i >> left_over) & ((1 << bufbits) - 1); // Mask away all but the first bufbits bits. TODO: Fix this mask
		}

		out_bits_left = bufbits - left_over; // left_over <= bufbits - 1, therefore out_bits_left >= 1
		out_bits = (i & ((~uint32_t(0) >> 1) >> (out_bits_left - 1))); // This shift works for out_bits_left == [1, 32]
	}
}

template<typename DerivedT>
void basic_obitstream<DerivedT>::put_uint(uint32_t i, uint32_t bits)
{
	// Bits must be at most 32
	sassert(bits <= 32);
	
	if(bits == 0) // Need this special case, as lsb_mask is not defined for 0
		return;
		
	/* TODO: Reserve memory first and use unchecked put byte */
	
	i &= gvl::lsb_mask(bits); /* Clear bits that won't get written */
	if(out_bits_left > bits)
	{
		/* We can fit it in */
		out_bits_left -= bits; /* out_bits_left is now the amount to left shift */
		/* out_bits_left is the number of zeroes
		** after the last written bit. */
		out_bits |= i << out_bits_left;
	}
	else if(bits > out_bits_left)
	{
		bits -= out_bits_left; /* bits is now the amount to right shift (bits left to write afterwards) */
		/* bits will be in [1, 31] as out_bits_left is at least 1 */
		out_bits |= i >> bits;
		derived().put_byte(byte(out_bits));
		if(bits > 0)
			derived().allocate_out_byte();
			
		for(; bits >= 8; )
		{
			bits -= 8; /* bits is now the amount to right shift (bits left to write afterwards) */
			derived().put_byte(byte(i >> bits));
			if(bits > 0)
				derived().allocate_out_byte();
		}
		
		/* We know bits < 8, so we can just write in the rest into out_bits */
		out_bits = (i << (8 - bits));
		out_bits_left = 8 - bits;
	}
	else if(bits == out_bits_left)
	{
		/* Fitting exactly */
		out_bits |= i; /* No need to shift in place because it is already */
		derived().put_byte(byte(out_bits));
		out_bits = 0;
		out_bits_left = 8;
	}
	
}

template<typename DerivedT>
void basic_obitstream<DerivedT>::put_block(void const* ptr_, size_t len)
{
	uint8_t const* ptr = static_cast<uint8_t const*>(ptr_);
	uint8_t const* end;
	if(len == 0)
		return;
	end = ptr + len;

	/* TODO: Reserve memory first and use unchecked put byte */
	
	if(out_bits_left == 8)
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
		unsigned int right = 8 - left;
		
		do
		{
			unsigned int b = *ptr++;
			v += (b >> right);
			derived().put_byte(byte(v));
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
template<typename DerivedT>
void basic_obitstream<DerivedT>::put_trunc(uint32_t v, uint32_t count)
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
template<typename DerivedT>
void basic_obitstream<DerivedT>::put_unary(uint32_t v)
{
	for(; v >= 32; v -= 32)
		put_uint(0, 32);
	put_uint(1, v+1);
}

// NOTE: Untested
template<typename DerivedT>
void basic_obitstream<DerivedT>::put_golomb(uint32_t v, uint32_t m)
{
	uint32_t quot = v / m;
	uint32_t rem = v % m;
	
	put_unary(quot);
	put_trunc(rem, m);
}

// NOTE: Untested
template<typename DerivedT>
void basic_obitstream<DerivedT>::put_rice(uint32_t v, uint32_t shift)
{
	sassert(shift > 0);
	uint32_t quot = v >> shift;
	uint32_t rem = v & gvl::lsb_mask(shift);
	
	put_unary(quot);
	put_uint(rem, shift);
}
#endif

template<typename DerivedT>
void basic_obitstream<DerivedT>::put_debug_mark()
{
	put_uint((1<<15)|1, 16);
}


template<typename DerivedT>
void basic_ibitstream<DerivedT>::ignore(uint32_t bits)
{
	uint32_t cursor = 8 - in_bits_left;
	cursor += bits;
	
	uint32_t bytes_to_read = cursor / 8;
	
	if(bytes_to_read >= 1)
	{
		derived().ignore_bytes(bytes_to_read - 1);
		in_bits = derived().get_byte();
	}
	
	in_bits_left = 8 - (cursor % 8);
}

template<typename DerivedT>
uint32_t basic_ibitstream<DerivedT>::get()
{
	if(in_bits_left == 0)
	{
		in_bits = derived().get_byte();
		in_bits_left = 8;
	}
	
	return (in_bits >> --in_bits_left) & 1;
}


template<typename DerivedT>
uint32_t basic_ibitstream<DerivedT>::get_uint(uint32_t bits)
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
		
		for(; bits >= 8; )
		{
			bits -= 8; /* bits is now the amount to left shift (bits left to read afterwards) */
			v = (v << 8) | derived().get_byte();
		}
		
		if(bits > 0)
		{
			in_bits_left = 8 - bits;
			in_bits = derived().get_byte();
			v = (v << bits) | (in_bits >> in_bits_left);
		}
		else
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

template<typename DerivedT>
void basic_ibitstream<DerivedT>::get_block(void* ptr_, size_t len)
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
		unsigned int left = 8 - right;
		unsigned int v = in_bits << left;
		
		unsigned int b;
		do
		{
			b = derived().get_byte();
			v += (b >> right);
			*ptr++ = byte(v);
			v = (b << left);
		}
		while(ptr != end);
		
		in_bits = byte(b);
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
template<typename DerivedT>
uint32_t basic_ibitstream<DerivedT>::get_trunc(uint32_t count)
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
template<typename DerivedT>
uint32_t basic_ibitstream<DerivedT>::get_unary()
{
	uint32_t v = 0;
	for(; !get(); ++v)
		/* Nothing */;
	return v;
}

// NOTE: Untested
template<typename DerivedT>
uint32_t basic_ibitstream<DerivedT>::get_golomb(uint32_t m)
{
	uint32_t quot = get_unary();
	uint32_t rem = get_trunc(m);
	
	return quot * m + rem;
}

// NOTE: Untested
template<typename DerivedT>
uint32_t basic_ibitstream<DerivedT>::get_rice(uint32_t shift)
{
	sassert(shift > 0);
	
	uint32_t quot = get_unary();
	uint32_t rem = get_uint(shift);
	
	return (quot << shift) + rem;
}

#endif

template<typename DerivedT>
void basic_ibitstream<DerivedT>::get_debug_mark()
{
	uint32_t m = get_uint(16);
	passert(m == ((1<<15)|1), "Debug mark not found");
}

template<typename DerivedT>
void basic_ibitstream<DerivedT>::resetg()
{
	in_bits_left = 0;
	in_bits = 0;
}

template<typename DerivedT>
void basic_obitstream<DerivedT>::resetp()
{
	out_bits_left = 8;
	out_bits = 0;
}

template<typename DerivedT>
inline bool basic_ibitstream<DerivedT>::is_aligned()
{
	return in_bits_left == 0;
}

template<typename DerivedT>
inline bool basic_obitstream<DerivedT>::is_aligned()
{
	return out_bits_left == 8;
}

template<typename DerivedT>
inline uint8_t basic_ibitstream<DerivedT>::unsafe_aligned_get_byte()
{
	sassert(is_aligned()); // Must be aligned
	return derived().get_byte();
}

template<typename DerivedT>
inline void basic_obitstream<DerivedT>::unsafe_aligned_put_byte(byte w)
{
	sassert(is_aligned()); // Must be aligned
	derived().put_byte(w);
}

/*
template<typename DerivedT>
void basic_obitstream<DerivedT>::clear()
{
	resetp();
	derived().clear();
}

template<typename DerivedT>
void basic_ibitstream<DerivedT>::clear()
{
	resetg();
	derived().clear();
}*/

template<typename DerivedT>
void basic_obitstream<DerivedT>::finish()
{
	if(out_bits_left < 32)
	{
		derived().put_byte(byte(out_bits));
		out_bits = 0;
		out_bits_left = 32;
	}
}

} // namespace gvl

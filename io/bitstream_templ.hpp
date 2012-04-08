#include "bitstream.hpp"
#include "../support/debug.hpp"
#include "../support/platform.hpp"
#include <cstdio>

#if GVL_MSVCPP
// Disable "shift count negative or too big, undefined behavior".
// We make sure such cases are never reached.
#pragma warning(push)
#pragma warning(disable: 4293)
#endif

namespace gvl
{

template<typename DerivedT, int BufBytes>
void basic_obitstream<DerivedT, BufBytes>::put(uint32_t bit)
{
	uint32_t count = out_bit_count;
	unsigned int bits = out_bits | (bit << count);
	++count;

	if(count == out_buf_bits)
	{
		put_buf(bits);
		out_bits = 0;
		out_bit_count = 0;
	}
	else
	{
		out_bits = bits;
		out_bit_count = count;
	}
}

template<typename DerivedT, int BufBytes>
void basic_obitstream<DerivedT, BufBytes>::put_uint(uint32_t i, uint32_t bits)
{
	// Bits must be at most 32
	sassert(bits <= 32);
	sassert(bits > 0);
	passert((i & gvl::lsb_mask(bits)) == i, "unwritten bits must be 0");

	uint32_t written = out_bit_count;
	uint32_t buf = out_bits | (i << written);

	written += bits;

	if(written >= out_buf_bits)
	{
		put_buf(buf & out_buf_mask);
		written -= out_buf_bits;
		int consumed_bits = (bits - written); // [1, 32]
		buf = gvl::shr_1_32(i, consumed_bits);

		while(out_buf_bits < 32 && written >= out_buf_bits)
		{
			put_buf(i & out_buf_mask);
			written -= out_buf_bits;
			buf = (buf >> out_buf_bits);
		}
	}

	out_bits = buf;
	out_bit_count = written;
}

template<typename DerivedT, int BufBytes>
void basic_obitstream<DerivedT, BufBytes>::put_block(void const* ptr_, size_t len)
{
	uint8_t const* ptr = static_cast<uint8_t const*>(ptr_);
	uint8_t const* end;
	if(len == 0)
		return;
	end = ptr + len;

	for(; ptr != end; ++ptr)
	{
		put_uint(*ptr, 8);
	}
}

template<typename DerivedT, int BufBytes>
template<typename T, int BufBytes2>
void basic_obitstream<DerivedT, BufBytes>::put(basic_ibitstream<T, BufBytes2>& src_, int bits)
{
	T& src = src_.derived();

	// TODO: Faster case when both streams are aligned to eachother, i.e.
	// (BufBytes == BufBytes2 && out_bit_count == src.in_bit_count).
	while(bits > 32)
	{
		put_uint(src.get_uint(32), 32);
		bits -= 32;
	}

	if(bits > 0)
		put_uint(src.get_uint(bits), bits);
}

// NOTE: Untested
// Integer in [0, count)
template<typename BitWriter>
void put_trunc(BitWriter& writer, uint32_t v, uint32_t count)
{
	uint32_t bits = gvl::ceil_log2(count);
	uint32_t p2 = (1 << bits);
	uint32_t b = count - p2;
	if(v < p2 - b)
		writer.put_uint(v, bits);
	else
		writer.put_uint(v + b, bits + 1);
}

// NOTE: Untested
template<typename BitWriter>
void put_unary(BitWriter& writer, uint32_t v)
{
	for(; v >= 32; v -= 32)
		writer.put_uint(0, 32);
	writer.put_uint(1 << v, v + 1);
}

// NOTE: Untested
template<typename BitWriter>
void put_golomb(BitWriter& writer, uint32_t v, uint32_t m)
{
	uint32_t quot = v / m;
	uint32_t rem = v % m;

	put_unary(writer, quot);
	put_trunc(writer, rem, m);
}

// NOTE: Untested
template<typename BitWriter>
void put_rice(BitWriter& writer, uint32_t v, uint32_t shift)
{
	sassert(shift > 0);
	uint32_t quot = v >> shift;
	uint32_t rem = v & gvl::lsb_mask(shift);

	put_unary(writer, quot);
	writer.put_uint(rem, shift);
}

template<typename BitWriter>
void put_exprice(BitWriter& writer, uint32_t v, uint32_t shift)
{
	sassert(shift > 0);
	uint32_t quot = v >> shift;
	uint32_t rem = v & gvl::lsb_mask(shift);

	uint32_t prefix = quot + 1;
	int prefix_bits = gvl::log2(prefix) + 1;

	put_unary(writer, prefix_bits);
	if(prefix_bits > 0)
		writer.put_uint(prefix - (1<<prefix_bits), prefix_bits);
	writer.put_uint(rem, shift);
}

inline uint32_t unary_max_size(uint32_t max_v)
{
	return max_v + 1;
}

inline uint32_t rice_max_size(uint32_t max_v, uint32_t shift)
{
	return unary_max_size(max_v >> shift) + shift;
}

template<typename DerivedT, int BufBytes>
void basic_obitstream<DerivedT, BufBytes>::put_debug_mark()
{
	put_uint((1<<15)|1, 16);
}


template<typename DerivedT, int BufBytes>
void basic_ibitstream<DerivedT, BufBytes>::ignore(uint32_t bits)
{
	uint32_t cursor = in_bit_count;
	cursor += bits;

	uint32_t bufs_to_read = cursor / in_buf_bits;

	if(bufs_to_read >= 1)
	{
		derived().ignore_bufs(bufs_to_read - 1);
		in_bits = get_buf();
	}

	in_bit_count = (cursor % in_buf_bits);
}

template<typename DerivedT, int BufBytes>
uint32_t basic_ibitstream<DerivedT, BufBytes>::get()
{
	if(in_bit_count == in_buf_bits)
	{
		in_bits = get_buf();
		in_bit_count = 0;
	}

	return (in_bits >> in_bit_count++) & 1;
}

template<typename DerivedT, int BufBytes>
uint32_t basic_ibitstream<DerivedT, BufBytes>::get_uint(uint32_t bits)
{
	// Bits must be at most 32
	sassert(bits <= 32);
	sassert(bits > 0);

	uint32_t read = in_bit_count;
	bitbuf_t buf = in_bits;

	uint32_t v;
	uint32_t new_read = read + bits;

	if(new_read <= in_buf_bits)
	{
		// read in [2, 31]
		in_bit_count = new_read;
		v = buf >> read;
	}
	else
	{
		// read in [1, 32]
		v = gvl::shr_1_32(buf, read);

		buf = get_buf();
		uint32_t written = in_buf_bits - read; // Bits written to v
		v |= buf << written;
		written += in_buf_bits;

		while(in_buf_bits < 32 && written < bits)
		{
			buf = get_buf();
			written += in_buf_bits;
			v |= buf << written;
		}

		in_bit_count = in_buf_bits - (written - bits); // Didn't use any surplus bits
		in_bits = buf;
	}

	return v & gvl::lsb_mask(bits);
}

template<typename DerivedT, int BufBytes>
void basic_ibitstream<DerivedT, BufBytes>::get_block(void* ptr_, size_t len)
{
	if(len == 0)
		return;

	uint8_t* ptr = static_cast<uint8_t*>(ptr_);
	uint8_t* end = ptr + len;

	for(; ptr != end; ++ptr)
		*ptr = get_uint(8);
}

template<typename BitReader>
uint32_t get_trunc(BitReader& reader, uint32_t count)
{
	uint32_t bits = gvl::ceil_log2(count);
	uint32_t p2 = (1 << bits);
	uint32_t b = count - p2;
	uint32_t v = reader.get_uint(bits);
	uint32_t get();
	if(v < p2 - b)
		return v;
	else
		return ((v << 1) + get()) - b;
}

// NOTE: Untested
template<typename BitReader>
uint32_t get_unary(BitReader& reader)
{
	uint32_t v = 0;
	for(; !reader.get(); ++v)
		/* Nothing */;
	return v;
}

// NOTE: Untested
template<typename BitReader>
uint32_t get_golomb(BitReader& reader, uint32_t m)
{
	uint32_t quot = get_unary(reader);
	uint32_t rem = get_trunc(reader, m);

	return quot * m + rem;
}

// NOTE: Untested
template<typename BitReader>
uint32_t get_rice(BitReader& reader, uint32_t shift)
{
	sassert(shift > 0);

	uint32_t quot = reader.get_unary();
	uint32_t rem = reader.get_uint(shift);

	return (quot << shift) + rem;
}


template<typename DerivedT, int BufBytes>
void basic_ibitstream<DerivedT, BufBytes>::get_debug_mark()
{
	uint32_t m = get_uint(16);
	passert(m == ((1<<15)|1), "Debug mark not found");
}

template<typename DerivedT, int BufBytes>
void basic_ibitstream<DerivedT, BufBytes>::resetg()
{
	in_bit_count = in_buf_bits;
	in_bits = 0;
}

template<typename DerivedT, int BufBytes>
void basic_obitstream<DerivedT, BufBytes>::resetp()
{
	out_bit_count = 0;
	out_bits = 0;
}

template<typename DerivedT, int BufBytes>
void basic_obitstream<DerivedT, BufBytes>::finish()
{
	if(out_bit_count > 0)
	{
		put_buf(out_bits);
		out_bits = 0;
		out_bit_count = 0;
	}
}

template<typename DerivedT, int BufBytes>
template<int ByteAlignment>
void basic_obitstream<DerivedT, BufBytes>::finish_aligned()
{
	int const bit_alignment = ByteAlignment * 8;
	if(out_bit_count > 0)
	{
		while(true)
		{
			put_chunk<int>(out_bits & GVL_BITMASK(bit_alignment));
			if(out_bit_count <= bit_alignment)
				break;
			out_bit_count -= bit_alignment;
			out_bits >>= bit_alignment;
		}

		out_bits = 0;
		out_bit_count = 0;
	}
}

template<typename DerivedT, int BufBytes>
template<int C>
unsigned int basic_ibitstream<DerivedT, BufBytes>::get_chunk()
{
	bitbuf_t v = derived().get_byte();
	for(int i = 1; i < C; ++i)
		v = (v << 8) | derived().get_byte();
	return v;
}

template<typename DerivedT, int BufBytes>
template<int C>
void basic_ibitstream<DerivedT, BufBytes>::ignore_chunks(int c)
{
	derived().ignore_bytes(c * C);
}

template<typename DerivedT, int BufBytes>
template<int C>
void basic_obitstream<DerivedT, BufBytes>::put_chunk(bitbuf_t v)
{
	for(int i = C - 1; i >= 0; --i)
		derived().put_byte(v >> (i*8));
}

} // namespace gvl

#if GVL_MSCVPP
#pragma warning(pop)
#endif

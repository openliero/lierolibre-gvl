#ifndef GVL_GASH_HPP
#define GVL_GASH_HPP

#include "../support/cstdint.hpp"

namespace gvl
{

struct gash
{
	int const block_size = 8;
	
	gash()
	{
		uint64_t accum = 1;
		for(int i = 0; i < 8; ++i)
		{
			accum *= 0x579d16a0ull;
			accum += 1;
			d[i] = accum;
		}
	}
	
	void process(uint64_t* n)
	{
		d[0] ^= n[0];
		d[1] ^= n[1];
		d[2] ^= n[2];
		d[3] ^= n[3];
		d[4] ^= n[4];
		d[5] ^= n[5];
		d[6] ^= n[6];
		d[7] ^= n[7];
		
		for(int i = 0; i < 8; ++i)
			round();
		
		d[0] += n[0];
		d[1] += n[1];
		d[2] += n[2];
		d[3] += n[3];
		d[4] += n[4];
		d[5] += n[5];
		d[6] += n[6];
		d[7] += n[7];
	}
	
	void round()
	{
		d[0] -= d[5];
		d[1] -= d[6];
		d[2] -= d[7];
		d[3] += d[0];
		d[4] += d[1];
		d[5] += d[2];
		d[6] ^= d[3];
		d[7] ^= d[4];
		
		d[5] = rot(d[5], 7);
		d[6] = rot(d[6], 17);
		d[7] = rot(d[7], 37);
		
		std::swap(d[0], d[4]);
		std::swap(d[2], d[5]);
		std::swap(d[3], d[7]);
	}
	
	uint64_t d[8];
};

template<typename Hash>
struct hash_accumulator
{
	void ui8(uint8_t v)
	{
		bit_n -= 8;
		cur |= (uint64_t(v) << bit_n);
		if(bit_n == 0)
		{
			dump_cur();
		}
	}
	
	void dump_cur()
	{
		bit_n = 64;
		buf[word_n++] = cur;
		cur = 0;
		if(word_n == Hash::block_size)
		{
			hash.process(buf);
			word_n = 0;
		}
	}
	
	void ui32(uint32_t v)
	{
		if(bit_n >= 32)
		{
			bit_n -= 32;
			cur |= (uint64_t(v) << bit_n);
			if(bit_n == 0)
				dump_cur();
		}
		else
		{
			int left = bit_n;
			cur |= v >> (32 - left);
			dump_cur();
			bit_n = 64 - (32 - left);
			cur = uint64_t(v) << bit_n;
		}
	}
	
	void flush()
	{
		// Pad with one followed by zeroes
		ui8(0x80);

		// Flush cur
		if(bit_n < 64)
		{
			buf[word_n++] = cur;
		}
		
		// Flush buf
		if(word_n > 0)
		{
			// Pad with 0
			for(int i = word_n; i < Hash::block_size; ++i)
			{
				buf[i] = 0;
			}
			hash.process(buf);
		}
	}
	
	hash_accumulator()
	: cur(0)
	{
	}
	
	Hash hash;
	uint64_t buf[Hash::block_size];
	int bit_n;
	int word_n;
	uint64_t cur;
};

}

#endif // GVL_GASH_HPP

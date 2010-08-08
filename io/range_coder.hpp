#ifndef UUID_A26A0B9FB41F438809FE82A839907D66
#define UUID_A26A0B9FB41F438809FE82A839907D66

#include <cmath>
#include <cstdlib>
#include <cstddef>
#include "../support/debug.hpp"
#include "../support/cstdint.hpp"

#ifdef _MSCVER
#define HAS_ULONGLONG
#define HAS_X86IASM
#endif

namespace gvl
{

struct range_coder_defs
{
	typedef uint32_t range_value;
		
	static range_value const ValueBits = 32;
	static range_value const TopValue = (1u << (ValueBits - 1));
	static range_value const MaxFreq = (1u << (ValueBits - 13));
	static int const ShiftBits = ValueBits - 9;
	static int const ExtraBits = (ValueBits - 2) % 8 + 1;
	static range_value const BottomValue = (TopValue >> 8);

	inline range_value muldiv32(range_value v, range_value s)
	{
		return (uint64_t(v) * s) >> 32;
	}
};




template<typename DerivedT>
struct range_coder : range_coder_defs
{
	range_coder(uint8_t header = 0)
	: low(0), range(TopValue)
	, follow_bytes(0), byte_count(0)
	, entropy(0.0), buffer(header), first(true)
	, debug(false)
	{
		
	}
	
	DerivedT& derived()
	{ return *static_cast<DerivedT*>(this); }

	void renormalize()
	{
		while(range <= BottomValue)
		{
			if (low < (0xff << ShiftBits))
			{
				if(!first)
					derived().put_byte(buffer);

				for(; follow_bytes; follow_bytes--)
					derived().put_byte(0xff);
				buffer = static_cast<uint8_t>(low >> ShiftBits);
				first = false;
			}
			else if (low & TopValue)
			{
				if(!first)
					derived().put_byte(buffer + 1);

				for(; follow_bytes; follow_bytes--)
					derived().put_byte(0);
				buffer = static_cast<uint8_t>(low >> ShiftBits);
				first = false;
			} else
				++follow_bytes;

			range <<= 8;
			low = (low << 8) & (TopValue - 1);
			++byte_count;
		}
	}

	void encode_range(range_value sym_low, range_value sym_range, range_value max)
	{
		assert(sym_range > 0);
		assert(max > sym_low);

#ifdef GVL_TRACK_ENTROPY
		entropy -= log(double(sym_range) / max);
#endif

		uint32_t r = range / max;
		uint32_t tmp = r * sym_low;
		low += tmp;
	
		if (sym_low + sym_range < max)
			range = r * sym_range;
		else
			range -= tmp;

		renormalize();
	}

	// max = 1 << shift
	void encode_range_shift(range_value sym_low, range_value sym_range, int shift, bool noID = false)
	{
		assert(sym_range > 0);
		assert(shift <= 16);
		assert((1 << shift) > sym_low);
		assert((1 << shift) >= sym_low + sym_range);

#ifdef TRACK_ENTROPY
		entropy -= log(double(sym_range) / double(1 << shift));
#endif

		uint32_t r = range >> shift;
		uint32_t tmp = r * sym_low;
		low += tmp;
	
		if ((sym_low + sym_range) >> shift)
			range -= tmp;
		else
			range = r * sym_range;

		renormalize();
	}

	void encode_marker(int value)
	{
		encode_range_shift(value, 1, 8, true);
	}

	void encode_raw_bit(int value)
	{
		assert(value == 0 || value == 1);

		uint32_t r = range >> 1;
		
		if(value > 0)
		{
			range -= r;
			low += r;
		}
		else
			range = r;

		renormalize();
	}

	// NOT WORKING
	void encode_bit(int value, int range0, int shift)
	{
		assert(value == 0 || value == 1);

		range_value r = range >> shift;
		range_value tmp = r * range0;
		
		if(value > 0)
		{
			range -= tmp;
			low += tmp;
		}
		else
			range = tmp;

		renormalize();
	}
	
	void encode_uint(unsigned int range_value)
	{
		unsigned int l = range_value & 0xFFFF;
		unsigned int h = (range_value >> 16) & 0xFFFF;
		encode_range_shift(l, 1, 16);
		encode_range_shift(h, 1, 16);
	}

	void encode_uint(unsigned int range_value, unsigned int max)
	{
		assert(range_value < max);
		
		if(max >= (1 << 16))
		{
			// TODO: Fix decoding of uint as well
			encode_range_shift(range_value & 0xFFFF, 1, 16);
			encode_range(range_value >> 16, 1, max >> 16);
		}
		else
			encode_range(range_value, 1, max);
	}

	void encode_byte(uint8_t range_value)
	{
		encode_range_shift(range_value, 1, 8);
	}

	double get_entropy()
	{
		return entropy / log(256);
	}

	void finish()
	{
		uint32_t tmp = (low >> ShiftBits) + 1;

		if (tmp > 0xff)
		{
			if(first)
				derived().put_byte(buffer);
			else
				derived().put_byte(buffer + 1);

			for(; follow_bytes; follow_bytes--)
				derived().put_byte(0);
		}
		else
		{
			derived().put_byte(buffer);
			for(; follow_bytes; follow_bytes--)
				derived().put_byte(0xff);
		}

		derived().put_byte(tmp & 0xff);
	}

	range_value low;
	range_value range;
	uint8_t buffer;
	std::size_t follow_bytes;
	std::size_t byte_count;
	double entropy;
	bool first;
	bool debug;
};

#if 0
#define self (static_cast<DerivedT*>(this))

template<typename DerivedT>
struct bit_range_coder_impl
{
	typedef uint32_t range_value;
		
	static range_value const ValueBits = 32;
	static range_value const TopValue = (1u << (ValueBits - 1));
	
	bit_range_coder_impl()
	: low(0), range(TopValue)
	, byte_count(0)
#ifdef VL_TRACK_ENTROPY
	, entropy(0.0)
#endif
	{
		
	}

	void renormalize()
	{
		while(((low ^ high) & 0xff000000) == 0)
		{
			put_byte(low >> 24);
			low = low << 8;
			high = (high << 8) + 0xff;
		}
	}

	void encode_raw_bit(int value)
	{
		sassert(value == 0 || value == 1);

		range_value middle = low + ((high - low) >> 2);
		
		sassert(middle >= low && middle < high);
		
		if(value)
			high = middle;
		else
			low = middle + 1;

		renormalize();
	}

	void encode_bit(int value, range_value prob1)
	{
#ifdef VL_TRACK_ENTROPY
		entropy -= std::log(prob1 / 4294967296.0);
#endif

		sassert(value == 0 || value == 1);

		range_value middle = low + muldiv32(high - low, prob1);
		
		sassert(middle >= low && middle < high);
		
		if(value)
			high = middle;
		else
			low = middle + 1;

		renormalize();
	}
	
	double get_entropy()
	{
		return entropy / log(256);
	}

	void finish()
	{
		renormalize();
		
		put_byte(high >> 24);
	}

	void put_byte(uint8_t b)
	{
		self->put_byte(b);
	}

	range_value low;
	range_value high;
#ifdef VL_TRACK_ENTROPY
	double entropy;
#endif
};


template<typename DerivedT>
struct bit_range_decoder_impl
{
	typedef uint32_t range_value;
		
	static range_value const ValueBits = 32;
	static range_value const TopValue = (1u << (ValueBits - 1));
	
	bit_range_decoder_impl()
	: low(0), range(TopValue)
	, byte_count(0)
#ifdef VL_TRACK_ENTROPY
	, entropy(0.0)
#endif
	{
		
	}

	void renormalize()
	{
		while(((low ^ high) & 0xff000000) == 0)
		{
			put_byte(low >> 24);
			low = low << 8;
			high = (high << 8) + 0xff;
			x = (x << 8) + derived().get_byte();
		}
	}
	
	void start()
	{
		x = 0;
		
		for(int i = 0; i < 4; ++i)
		{
			x = (x << 8) + derived().get_byte();
		}
	}

	int decode_raw_bit()
	{
		sassert(value == 0 || value == 1);

		range_value middle = low + ((high - low) >> 2);
		
		sassert(middle >= low && middle < high);
		
		int value = 0;
		
		if(x <= middle)
		{
			high = middle;
			value = 1;
		}
		else
			low = middle + 1;

		renormalize();
		
		return value;
	}


	int decode_bit(range_value prob1)
	{
#ifdef VL_TRACK_ENTROPY
		entropy -= std::log(prob1 / 4294967296.0);
#endif

		sassert(value == 0 || value == 1);

		range_value middle = low + scale(high - low, prob1);
		
		sassert(middle >= low && middle < high);
		
		int value = 0;
		
		if(x <= middle)
		{
			if(x + uncertainty > middle)
				uncertain();
			high = middle;
			value = 1;
		}
		else if(x + uncertainty <= middle)
		{
			uncertain();
		}
		else
		{
			low = middle + 1;
		}

		renormalize();
		
		return value;
	}
	
	double get_entropy()
	{
		return entropy / log(256);
	}

	void finish()
	{
		
	}

	uint8_t derived().get_byte()
	{
		return self->derived().get_byte();
	}

	range_value low;
	range_value high;
	range_value x;

#ifdef VL_TRACK_ENTROPY
	double entropy;
#endif
};

#undef self
#endif

template<typename DerivedT>
struct range_decoder : range_coder_defs
{
	range_decoder()
	: buffer(0)
	{
		header = derived().get_byte();
		buffer = derived().get_byte();
		low = buffer >> (8 - ExtraBits);
		range = 1 << ExtraBits;
	}
	
	DerivedT& derived()
	{ return *static_cast<DerivedT*>(this); }

	void renormalize()
	{
		while(range <= BottomValue)
		{
			low = (low << 8) | ((buffer << ExtraBits) & 0xff);
			buffer = derived().get_byte();
			low |= buffer >> (8 - ExtraBits);
			range <<= 8;
		}
	}

	range_value decode(range_value max)
	{
		renormalize();

		help = range / max;
		range_value tmp = low / help;

		return (tmp >= max ? max - 1 : tmp);
	}

	range_value decode_shift(int shift)
	{
		renormalize();

		help = range >> shift;
		range_value tmp = low / help;

		return (tmp >> shift ? (1 << shift) - 1 : tmp);
	}

	int decode_raw_bit()
	{
		renormalize();
		
		range_value r = range >> 1;

		range_value sublow = low - r;
		range_value subrange = range - r;
		bool v = low >= r;
		if(!v)
		{
			range = r;
		}
		else
		{
			range = subrange;
			low = sublow;
		}
		
		return v;
/*
		if(low < r)
		{
			range = r;
			return 0;
		}
		else
		{
			range -= r;
			low -= r;
			return 1;
		}*/
	}
	
	// TODO: NOT WORKING, even though every time I derive this I get the same result.
	int decode_bit(int range0, int shift)
	{
		passert(false, "Not working");
		renormalize();

		range_value r = range >> shift;
		range_value tmp = r * range0;

		if(low < r)
		{
			range = tmp;
			return 0;
		}
		else
		{
			range -= tmp;
			low -= tmp;
			return 1;
		}
	}
	
	unsigned int decode_marker()
	{
		unsigned int t = decode_shift(8, true);
		range_decoded(t, 1, 1<<8);
		return t;
	}

	unsigned int decode_uint()
	{
		unsigned int l = decode_shift(16);
		range_decoded(l, 1, 1<<16);
		unsigned int h = decode_shift(16);
		range_decoded(h, 1, 1<<16);
		return l | (h << 16);
	}

	unsigned int decode_uint(unsigned int max)
	{
		unsigned int v = decode(max);
		range_decoded(v, 1, max);
		return v;
	}

	void range_decoded(range_value sym_low, range_value sym_range, range_value max)
	{
		range_value tmp = help * sym_low;
		low -= tmp;
		if (sym_low + sym_range < max)
			range = help * sym_range;
		else
			range -= tmp;
	}

	void finish()
	{
		
	}

	// Model tools
	
	std::size_t find_sym_by_cfreq(unsigned int c, unsigned int* cfreq, std::size_t len)
	{
		assert(c >= 0);
		
		std::size_t i = 1;
		for(; cfreq[i] > c; ++i)
			;

		return i - 1;
	/*
		std::size_t low = 1;
		std::size_t high = len;
		std::size_t last = 0;

		do
		{
			std::size_t mid = (low + high) / 2;
			if(cfreq[mid] < c)
			{
				high = mid - 1;
				last = mid;
			}
			else if(cfreq[mid] > c)
			{
				low = mid + 1;
			}
			else
				return mid - 1;
		} while(high >= low);

		assert(last != 0);
		assert(cfreq[last] <= c);
		assert(cfreq[last - 1] > c);

		return last - 1;*/
	}
	
	std::size_t decode_by_cfreq(unsigned int* freq, unsigned int* cfreq, std::size_t len)
	{
		if(cfreq[0] == 0)
			return '?';
			
		unsigned int v = decode(cfreq[0]);
		std::size_t sym = find_sym_by_cfreq(v, cfreq, len);
		
		sassert(freq[sym] > 0);
		range_decoded(cfreq[sym + 1], freq[sym], cfreq[0]);
		return sym;
	}

	range_value low;
	range_value range;
	range_value help;
	uint8_t buffer;
	uint8_t header;
};

}

#endif // UUID_A26A0B9FB41F438809FE82A839907D66

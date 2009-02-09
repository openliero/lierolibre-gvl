#ifndef UUID_706A361125654B859FE4FABB7193DE74
#define UUID_706A361125654B859FE4FABB7193DE74

#include "bitstream.hpp"
#include "../support/cstdint.hpp"
#include <vector>
#include <stdexcept>

namespace gvl
{

struct vector_bitstream;

template<typename RandomAccessIterator>
struct range_ibitstream
	: gvl::basic_ibitstream<range_ibitstream<RandomAccessIterator> >
	// obitstream that throws on output operations.
	// Needed to normalize interface for templates (notably uevent<>)
	, gvl::dummy_obitstream
{
	range_ibitstream(
		RandomAccessIterator cur,
		RandomAccessIterator end)
	: cur(cur)
	, end(end)
	{
	}
		
	range_ibitstream(vector_bitstream const&);
	
	// Default copy ctor and op= are fine
	
	uint8_t get_byte()
	{
		if(cur == end)
		{
			throw std::runtime_error("Bitstream exhausted");
		}
		return *cur++;
	}
	
	void ignore_bytes(std::size_t c)
	{
		if(end - cur < c)
			throw std::runtime_error("Bitstream exhausted");
		cur += c;
	}
	
	std::size_t bits_left() const
	{
		return (end - cur) * 8 + this->in_bits_left;
	}
	
	RandomAccessIterator cur;
	RandomAccessIterator end;
};

typedef range_ibitstream<uint8_t const*> array_ibitstream;

/*
struct array_ibitstream
	: gvl::basic_ibitstream<array_ibitstream>
{
	array_ibitstream(
		uint8_t const* ptr,
		uint8_t const* end)
	: ptr(ptr)
	, end(end)
	{
	}
		
	array_ibitstream(vector_bitstream const&);
	
	// Default copy ctor and op= are fine
	
	uint8_t get_byte()
	{
		if(ptr == end)
		{
			throw std::runtime_error("Bitstream exhausted");
		}
		return *ptr++;
	}
	
	std::size_t bits_left() const
	{
		return (end - ptr) * 8 + in_bits_left;
	}
	
	uint8_t const* ptr;
	uint8_t const* end;
};*/

struct vector_bitstream
	: gvl::basic_ibitstream<vector_bitstream>
	, gvl::basic_obitstream<vector_bitstream>
{
	vector_bitstream()
	: in_pos(0), size_(0)
	{
	}
	
	// NOTE: Caller is expected to verify that the number of bits
	// left in (bs) is at least (size_init).
	vector_bitstream(array_ibitstream& bs, std::size_t size_init)
	: gvl::basic_ibitstream<vector_bitstream>(bs)
	, in_pos(0)
	, size_(size_init)
	, data(bs.cur, bs.cur + bs.in_bytes_required(size_))
	{
	}
	
	void update_size()
	{
		size_ = out_bits_written(data.size());
	}
	
	void put_byte(uint8_t v)
	{
		data.push_back(v);
	}
	
	uint8_t get_byte()
	{
		if(in_pos == data.size())
		{
			throw std::runtime_error("Bitstream exhausted");
		}
		return data[in_pos++];
	}
	
	void ignore_bytes(std::size_t c)
	{
		if(data.size() - in_pos < c)
			throw std::runtime_error("Bitstream exhausted");
		in_pos += c;
	}
	
	std::size_t size() const
	{
		//update_size(); // TODO: Try to do this less often
		return out_bits_written(data.size());
	}
	
	void swap(vector_bitstream& b)
	{
		gvl::basic_ibitstream<vector_bitstream>::swap(b);
		gvl::basic_obitstream<vector_bitstream>::swap(b);
		
		std::swap(in_pos, b.in_pos);
		std::swap(size_, b.size_);
		data.swap(b.data);
	}
	
	// To be able to overload put
	using gvl::basic_obitstream<vector_bitstream>::put;
	
	// NOTE: Does not consume anything from bs
	void put(vector_bitstream& bs)
	{
		put_uint(bs.in_bits, bs.in_bits_left);
		for(std::size_t i = bs.in_pos; i < bs.data.size(); ++i)
			put_uint(bs.data[i], 8);
		put_uint(bs.out_bits >> bs.out_bits_left, 8 - bs.out_bits_left);
	}
	
	void rewindg()
	{
		resetg();
		in_pos = 0;
	}

	void clear()
	{
		resetp();
		resetg();
		data.clear();
		rewindg();
	}
	
	std::size_t in_pos;
	std::size_t size_;
	std::vector<uint8_t> data;
	
private:
	// Non-copyable
	vector_bitstream(vector_bitstream const&);
	vector_bitstream& operator=(vector_bitstream const&);
};

template<typename RandomAccessIterator>
inline range_ibitstream<RandomAccessIterator>::range_ibitstream(vector_bitstream const& bs)
: gvl::basic_ibitstream<range_ibitstream<RandomAccessIterator> >(bs)
, cur(bs.data.empty() ? 0 : (&bs.data[0] + bs.in_pos))
, end(cur + bs.in_bytes_required(bs.size()))
{
}

/*
inline array_ibitstream::array_ibitstream(vector_bitstream const& bs)
: gvl::basic_ibitstream<array_ibitstream>(bs)
, ptr(bs.data.empty() ? 0 : (&bs.data[0] + bs.in_pos))
, end(ptr + bs.in_bytes_required(bs.size()))
{
}*/

} // namespace psync

#endif // UUID_706A361125654B859FE4FABB7193DE74

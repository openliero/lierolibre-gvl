#ifndef UUID_E882406B58BE4B04808FB79776E9882A
#define UUID_E882406B58BE4B04808FB79776E9882A

#include "debug.hpp"

namespace gvl
{

// Bit set with very cheap clearing

template<typename Tag>
struct cheap_flags_item
{
	cheap_flags_item()
	: last_iter_set(-1)
	{
	}
	
	int last_iter_set;
};

template<typename Tag>
struct cheap_flags
{
	cheap_flags()
	: iter_count(0)
	{
	}
	
	bool operator[](cheap_flags_item<Tag> const& item)
	{
		return item.last_iter_set >= iter_count;
	}
	
	void set(cheap_flags_item<Tag>& item)
	{
		item.last_iter_set = iter_count;
	}
	
	void reset(cheap_flags_item<Tag>& item)
	{
		item.last_iter_set = -1;
	}
	
	void clear()
	{
		passert((iter_count + 1) > iter_count, "Clear limit reached");
		++iter_count;
	}

	int iter_count;
};

struct cheap_flags_dummy_tag;

struct cheap_flags_vector_item : cheap_flags_item<cheap_flags_dummy_tag>
{
};

struct cheap_flags_vector : cheap_flags<cheap_flags_dummy_tag>
{
	// TODO
	std::vector<cheap_flags_vector_item> items;
};

} // namespace gvl

#endif // UUID_E882406B58BE4B04808FB79776E9882A

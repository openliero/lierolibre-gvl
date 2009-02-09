#include <gvl/tut/tut.hpp>

#include <gvl/io/bitstream.hpp>
#include <gvl/io/common_bitstream.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/cstdint.hpp>
#include <gvl/support/macros.hpp>
#include <functional>
#include <vector>
#include <utility>

namespace tut
{

struct bitstream_data
{
	
};

typedef test_group<bitstream_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::bitstream");
} // namespace

namespace tut
{

template<typename T, std::size_t N>
std::size_t array_size(T(&)[N])
{
	return N;
}

template<>
template<>
void object::test<1>()
{
	typedef std::vector<std::pair<uint32_t, uint32_t> > SampleVec;
	SampleVec samples;
	
	gvl::tt800 rand(0);
	
	for(int i = 0; i < 1000; ++i)
	{
		uint32_t bits = rand.range(0, 33);
		uint32_t v;
		if(bits == 32)
			v = rand();
		else
			v = rand.range(0u, 1u<<bits);
		
		samples.push_back(std::make_pair(bits, v));
	}
	
	gvl::vector_bitstream bs;
	
	FOREACH(SampleVec, i, samples)
	{
		bs.put_uint(i->second, i->first);
	}
	
	bs.finish();
			
	FOREACH(SampleVec, i, samples)
	{
		uint32_t v = bs.get_uint(i->first);
		ensure("integers encoded right", v == i->second);
	}
	
	bs.rewindg();
	
	// Get with interjected ignores
	int ignoreLen = 0;
	FOREACH(SampleVec, i, samples)
	{
		if(rand() & 1)
			ignoreLen += i->first;
		else
		{
			if(ignoreLen > 0)
			{
				bs.ignore(ignoreLen);
				ignoreLen = 0;
			}
			uint32_t v = bs.get_uint(i->first);
			ensure("integers encoded right", v == i->second);
			
		}
	}
	
	bs.clear();
	
	FOREACH(SampleVec, i, samples)
	{
		bs.put(i->second & 1);
	}
	
	bs.finish();
			
	FOREACH(SampleVec, i, samples)
	{
		uint32_t v = bs.get();
		ensure("bits encoded right", v == (i->second & 1));
	}
	
}

} // namespace tut

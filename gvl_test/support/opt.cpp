#include <gvl/tut/tut.hpp>

#include <gvl/support/opt.hpp>
#define GVL_PROFILE 1
#include <gvl/support/profile.hpp>
#include <gvl/math/cmwc.hpp>
#include <gvl/io/formatter.hpp>
#include <gvl/io/fstream.hpp>
#include <gvl/io/stream.hpp>

#include <numeric>

namespace tut
{

struct opt_data
{
	
};

typedef test_group<opt_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("opt.hpp");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
#if !GVL_PROFILE
	for(uint32_t y = 2; y < 1000000; y += 761)
	{
		gvl::prepared_division d(y);
		
		for(uint32_t x = 3; x < 1000000000; x += 1327)
		{
			uint32_t expected = x / y;
			uint32_t actual = d.quot(x);
			
			ensure("division was correct", expected == actual);
		}
		
		//std::cout << y << ',';
	}
#endif
}

template<>
template<>
void object::test<2>()
{
	gvl::stream_ptr cout(new gvl::fstream(stdout));
	gvl::stream_ptr cin(new gvl::fstream(stdin));
	gvl::stream_reader cin_reader(cin);
	gvl::stream_writer cout_writer(cout);
	gvl::formatter<> console(cin_reader, cout_writer);
	
	console << 100;
	
	cout_writer.flush();
	
#if GVL_PROFILE
	uint32_t divisors[512];
	gvl::mwc rand(1);
	
	for(int i = 0; i < 512; ++i)
	{
		divisors[i] = rand(3, 1337);
	}
	
	uint32_t sum = 0;
	
	{
		GVL_PROF_TIMER("prepared_division");
			
		for(int i = 0; i < 512; ++i)
		{
			gvl::prepared_division d(divisors[i]);
			
			uint32_t x = 0x13371337;
			
			for(int j = 0; j < 65536; ++j)
			{
				std::pair<uint32_t, uint32_t> p = d.quot_rem(x);
				x = p.first;
				sum += p.second;
			}
		}
	}
	
	{
		GVL_PROF_TIMER("normal division");
			
		for(int i = 0; i < 512; ++i)
		{
			uint32_t d = divisors[i];
			
			uint32_t x = 0x13371337;
			
			for(int j = 0; j < 65536; ++j)
			{
				x /= d;
				sum += (x % d);
			}
		}
	}
	
	{
		GVL_PROF_TIMER("static division");
			
		for(int i = 0; i < 512; ++i)
		{
			uint32_t x = 0x13371337;
			
			for(int j = 0; j < 65536; ++j)
			{
				x /= 1337;
				sum += (x % 1337);
			}
		}
	}
	
	std::cout << "Sum: " << sum << '\n';
	
	gvl::present_profile(std::cout);
#endif
}


} // namespace tut

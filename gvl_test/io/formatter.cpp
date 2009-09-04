#include <gvl/tut/tut.hpp>

#include <gvl/io/formatter.hpp>
#include <gvl/io/stream.hpp>
#include <gvl/containers/range.hpp>
#define GVL_PROFILE 1
#include <gvl/support/profile.hpp>
#include <gvl/system/system.hpp>

namespace tut
{

struct formatter_data
{
	
};

typedef test_group<formatter_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::formatter");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
	using namespace gvl;
	
	typedef delimited_iterator_range<uint8_t*> buffer_writer;
	
	uint8_t buffer[32];
	buffer_writer orig_writer(buffer, buffer + 32);
	
	uint32_t const limit = 1000000;
	
	uint32_t const base = 16;
	uint32_t volatile vbase_ = base;
	uint32_t vbase = vbase_;
	
	gvl::sleep(1000);
	
	{
		GVL_PROF_TIMER("static uint -> ascii");
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			buffer_writer writer = orig_writer;
			uint_to_ascii_base<base>(writer, i);
		}
	}
	
	{
		GVL_PROF_TIMER("dynamic base uint -> ascii");
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			buffer_writer writer = orig_writer;
			uint_to_ascii(writer, i, vbase);
		}
	}

	{
		GVL_PROF_TIMER("itoa");
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			_itoa(i, (char*)buffer, vbase);
		}
	}
	
	
}

struct string_writer
{
	string_writer(std::string& str_init)
	: str(str_init)
	{
	}
	
	void put(uint8_t const* p_, std::size_t count)
	{
		char const* p = reinterpret_cast<char const*>(p_);
		str.insert(str.end(), p, p + count);
	}
	
	void put(uint8_t x)
	{
		str.push_back(static_cast<char>(x));
	}
	
	std::string& str;
};

template<>
template<>
void object::test<2>()
{
	using namespace gvl;
	
	shared_ptr<brigade_buffer> dest(new brigade_buffer);
	stream_writer writer(dest);
	format_writer<> fwriter(writer);
	
	std::stringstream ss;
	
	uint32_t limit = 2000000;
	
	std::string a, b, c;
	
	{
		GVL_PROF_TIMER("stringstream");
				
		for(uint32_t i = 0; i < limit; ++i)
		{
			ss << "Hello " << i << '\n';
		}
		
		b = ss.str();
	}
	
	{
		c.reserve(26888890);
		GVL_PROF_TIMER("direct");
		char const* const hello = "Hello ";
		uint8_t buf[32+6];
		std::memset(buf, 0, sizeof(buf));
		std::memcpy(buf, hello, 6);
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			unsafe_delimited_iterator_range<uint8_t*> buf_range(buf + 6, buf + 32 + 6);
			uint_to_ascii_base<10>(buf_range, i);
			*buf_range.i++ = '\n';
			c.append((char const*)buf);
		}
	}
	
	{
		uint8_t* data = new uint8_t[26888890];
		
		GVL_PROF_TIMER("c str");
		char const* const hello = "Hello ";
		uint8_t* p = data;
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			*reinterpret_cast<uint32_t*>(p) = 'lleH';
			p += 4;
			*reinterpret_cast<uint32_t*>(p) = '   o';
			p += 2;
			unsafe_delimited_iterator_range<uint8_t*> buf_range(p, p + 32);
			uint_to_ascii_base<10>(buf_range, i);
			p = buf_range.i;
			*p++ = '\n';
		}
	}
	
	{
		GVL_PROF_TIMER("brigade_buffer");
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			fwriter << "Hello " << i << '\n';
		}
		
		fwriter.flush();
		
		dest->to_str(a);
	}
	
	ensure("strings are equal", a == b && b == c);
	std::cout << a.size() << std::endl;
}

} // namespace tut


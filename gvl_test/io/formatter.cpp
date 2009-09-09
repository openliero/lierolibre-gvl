#include <gvl/tut/tut.hpp>

#include <gvl/io/encoding.hpp>
#include <gvl/io/stream.hpp>
#include <gvl/containers/range.hpp>
#undef GVL_PROFILE
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
#if GVL_PROFILE
	using namespace gvl;
	
	typedef unsafe_delimited_iterator_range<uint8_t*> buffer_writer;
	
	uint8_t buffer[32];
	buffer_writer orig_writer(buffer, buffer + 32);
	
	uint32_t const limit = 1000000;
	
	uint32_t const base = 10;
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
#endif
}

struct string_writer : gvl::basic_text_writer<string_writer>
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
	
	void flush()
	{
		// Nothing
	}
	
	std::string& str;
};

template<>
template<>
void object::test<2>()
{
#if GVL_PROFILE
	using namespace gvl;
	
#ifdef NDEBUG
	uint32_t limit = 2000000;
#else
	uint32_t limit = 20000;
#endif
	
	std::string a, b, c, d;
	
	{
		std::stringstream ss;
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
		shared_ptr<memory_stream> dest(new memory_stream);
		octet_stream_writer writer(dest);
		
		GVL_PROF_TIMER("octet_stream_writer + memory_stream");
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			writer << "Hello " << i << '\n';
		}
		
		writer.flush();
		
		dest->to_str(a);
	}
	
	{
		shared_ptr<memory_stream> dest(new memory_stream);
		octet_stream_writer writer(dest);
		
		GVL_PROF_TIMER("octet_stream_writer + memory_stream + unlimited bucket growth");
		writer.set_unlimited_bucket();
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			writer << "Hello " << i << '\n';
		}
		
		writer.flush();
		
		gvl::string str;
		dest->release_as_str(str);
	}
	
	{
		GVL_PROF_TIMER("string_writer");
		
		std::string str;
		string_writer swriter(str);
		
		for(uint32_t i = 0; i < limit; ++i)
		{
			swriter << "Hello " << i << '\n';
		}
		
		swriter.flush();
	}
	
	ensure("strings are equal", a == b && b == c);
#endif
}

} // namespace tut


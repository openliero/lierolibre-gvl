#include <gvl/tut/tut.hpp>

//#include <gvl/io/stream.hpp>
//#include <gvl/sockets/socketstream.hpp>
#include <gvl/io/stream.hpp>
#include <gvl/math/tt800.hpp>
//#include <gvl/support/algorithm.hpp>
#include <functional>

namespace tut
{

struct stream_data
{
	
};

typedef test_group<stream_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::stream");
} // namespace

namespace tut
{

template<typename T, std::size_t N>
std::size_t array_size(T(&)[N])
{
	return N;
}

/*
// A filter automatically pushes or pulls when pushed or pulled respectively
filter : sink_t, source_t
{
	put(bucket) // put writes data to sink if possible
	get(bucket) // get pulls data from source if necessary
	
	source_t* source;
	sink_t* sink;
}

// A stream pushes data to an unknown place and pulls it from the same place
stream : sink_t, source_t
{
	put(bucket) // 
	get(bucket) // 
}

// A brigade works like a filter that never reads/writes on demand, instead buffers all data in an internal structure
brigade : sink_t, source_t
{
	put(bucket) // 
	get(bucket) // 
}
*/

template<>
template<>
void object::test<1>()
{
/*
	gvl::shared_ptr<gvl::fstream> f(new gvl::fstream("foo.replay.gz"));
	gvl::shared_ptr<gvl::fstream> f2(new gvl::fstream("foo.replay"));
	gvl::shared_ptr<gvl::filter> z(new gvl::gzip_filter(f2, f));
	
	z.pump_all(); // pump_all reads from the source and writes to the sink
	*/
	
	using namespace gvl;
	
	stream_ptr source(new brigade_buffer());
	stream_ptr sink(new brigade_buffer());
	
	filter_ptr filter(new filter(source, sink));
	
	stream_writer writer(source);
	
	for(int i = 0; i < 10; ++i)
	{
		writer.put(1);
	}
	writer.flush();
	
	filter->pump();
	
#if 0
	char const txt[] = "hello";
	
	gvl::linked_vector<uint8_t> b;
	
	{
		gvl::linked_vector_temp<uint8_t, 5> b2;
		std::memcpy(b2.mut_data(), txt, 5);
		b = b2;
	}
	
	{
		gvl::stream s(new gvl::bucket_mem(b), 0);
		
		for(std::size_t i = 0; i < 3; ++i)
			ensure(s.get() == txt[i]);
			
		s.add_source(new gvl::bucket_mem("world", 5));
		
		gvl::linked_vector<uint8_t> d;
		s.buffer_sequenced(5, d);
	}


	gvl::socketstream ss("se.quakenet.org", 6667);
#endif
	
	//ensure("no leak", b->ref_count() == 1);
	
	/*
	gvl::filter::auto_read_result r(s.get_bucket());
	
	if(r.s == gvl::filter::status::ok)
	{
		
	}*/
}

} // namespace tut

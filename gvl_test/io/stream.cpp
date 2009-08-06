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

struct inc_filter : gvl::filter
{
	read_status apply(bool can_pull, bool flush = false, size_type amount = 0)
	{
		if(out_buffer.empty())
		{
			if(!can_pull)
				return read_blocking;
				
			read_status res = try_pull(amount);
			if(res != read_ok)
				return res;
		}
		
		while(!out_buffer.empty())
		{
			gvl::bucket* b = out_buffer.unlink_first();
			
			std::size_t s = b->size();
			gvl::bucket_data_mem* dest = gvl::bucket_data_mem::create(s, s);
			
			uint8_t const* p = b->get_ptr();
			
			for(std::size_t i = 0; i < s; ++i)
			{
				dest->data[i] = p[i] + 1;
			}
			
			in_buffer.append(new gvl::bucket(dest));
		}
		
		return read_ok;
	}
};

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
	
	stream_writer writer(source);
	
	for(int i = 0; i < 10; ++i)
	{
		writer.put(i);
	}
		
	writer.flush();
	
	gvl::shared_ptr<inc_filter> filter(new inc_filter());
	
	stream_reader reader(source);
	
	int i1 = reader.get();
	int i2 = reader.get();
	
	ensure(i1 == 0);
	ensure(i2 == 1);
	
	// Add a filter
	stream_ptr end = reader.detach();
	filter->attach_source(end);
	reader.attach(filter);
	
	int i3 = reader.get();
	int i4 = reader.get();
	
	ensure(i3 == 3);
	ensure(i4 == 4);
	
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

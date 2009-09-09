#include <gvl/tut/tut.hpp>

//#include <gvl/io/stream.hpp>
//#include <gvl/sockets/socketstream.hpp>
#include <gvl/io/stream.hpp>
#include <gvl/io/encoding.hpp>
#include <gvl/io/deflate_filter.hpp>
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

#if 0 // TODO: Return value is inaccurate of apply
struct inc_filter : gvl::filter
{
	read_status apply(apply_mode mode, size_type amount = 0)
	{
		if(out_buffer.empty() && mode == am_pulling)
		{
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
#endif

template<>
template<>
void object::test<1>()
{
	using namespace gvl;
	
	stream_ptr sink(new memory_stream());
	
	shared_ptr<deflate_filter> filter(new deflate_filter(true));
	filter->attach_sink(sink);
	
	octet_stream_writer writer(filter);
	
	uint8_t seq[] = {1, 2, 3, 4};
	
	for(int i = 0; i < 1000; ++i)
	{
		writer.put(13);
		writer.put(seq, 4);
	}
	
	writer.flush();
	writer.detach();
	
	shared_ptr<deflate_filter> filter2(new deflate_filter(false));
	filter2->attach_source(sink);
	
	octet_stream_reader reader(filter2);
	for(int i = 0; i < 1000; ++i)
	{
		int v = reader.get();
		ensure(v == 13);
		ensure(reader.get() == 1);
		ensure(reader.get() == 2);
		ensure(reader.get() == 3);
		ensure(reader.get() == 4);
	}
}

} // namespace tut

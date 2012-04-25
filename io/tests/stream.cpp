#include "../../test/test.hpp"

#include "../stream.hpp"
#include "../encoding.hpp"
#include "../deflate_filter.hpp"
#include "../../math/tt800.hpp"
#include "../../math/cmwc.hpp"
#include <functional>

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

GVLTEST_SUITE(gvl, stream)

GVLTEST(gvl, stream, deflate_filter)
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
		ASSERTEQ(v, 13);
		ASSERTEQ(reader.get(), seq[0]);
		ASSERTEQ(reader.get(), seq[1]);
		ASSERTEQ(reader.get(), seq[2]);
		ASSERTEQ(reader.get(), seq[3]);
	}
}

GVLTEST(gvl, stream, octet_stream_reader_get)
{
	gvl::stream_ptr str(new gvl::memory_stream());
	gvl::octet_stream_writer writer(str);

	gvl::mwc r;

	int const count = 4096;

	for(int i = 0; i < count; ++i)
	{
		writer.put(uint8_t(i & 0xff));
	}

	writer.flush();

	gvl::octet_stream_reader reader(str);

	for(int i = 0; i < count;)
	{
		int size = std::min(int(r(10) + 3), (count - i));
		uint8_t buf[64];

		reader.get(buf, size);

		int start = i;
		for(int off = 0; off < size; ++off)
		{
			ASSERTEQ((int)buf[off], (i & 0xff));
			++i;
		}
	}
}
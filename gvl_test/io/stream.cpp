/*
 * Copyright (c) 2010, Erik Lindroos <gliptic@gmail.com>
 * This software is released under the The BSD-2-Clause License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <tut/tut.hpp>

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
		ensure(reader.get() == seq[0]);
		ensure(reader.get() == seq[1]);
		ensure(reader.get() == seq[2]);
		ensure(reader.get() == seq[3]);
	}
}

} // namespace tut

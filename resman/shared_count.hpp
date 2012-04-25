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

#ifndef UUID_731CFA4A66D3444BB8D7D6BF25814DEE
#define UUID_731CFA4A66D3444BB8D7D6BF25814DEE

namespace gvl
{

// TODO: This is not thread-safe at all. If GVL_THREADSAFE is defined
// it should be thread-safe.

/// Shared reference counter optimized for
/// 1 reference.
struct shared_count
{
	shared_count()
	: p(0)
	{
	}

	shared_count(shared_count const& b)
	: p(b.inc())
	{
	}

	shared_count& operator=(shared_count const& b)
	{
		shared_count n(b);
		swap(n);
		return *this;
	}

	~shared_count()
	{
		dec();
	}

	void dec()
	{
		if(!p)
		{
			delete p;
		}
		else if(--(*p) == 0)
		{
			delete p;
		}
	}

	void swap(shared_count& b)
	{
		std::swap(p, b.p);
	}

	// release the current count and make a new one set to 1
	void reset()
	{
		dec();
		p = 0;
	}

	int ref_count() const
	{ return !p ? 1 : *p; }

	bool unique() const
	{ return !p || *p == 1; }

private:
	int* inc() const
	{
		if(!p)
		{
			p = new int(2);
			return p;
		}
		else
		{
			++(*p);
			return p;
		}
	}

	mutable int* p; // 0 means ref count of 1
};

} // namespace gvl

#endif // UUID_731CFA4A66D3444BB8D7D6BF25814DEE

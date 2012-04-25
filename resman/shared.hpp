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

#ifndef UUID_5F7548A068F9485B6759368B5BAE8157
#define UUID_5F7548A068F9485B6759368B5BAE8157

#include <algorithm>

namespace gvl
{

struct weak_ptr_common;

struct shared
{
	friend struct weak_ptr_common;

	shared()
	: _ref_count(1), _first(0)
	{

	}

	// const to allow shared_ptr<T const>
	void add_ref() const
	{
#if GVL_THREADSAFE
		#error "Not finished"
		// TODO: Interlocked increment
#else
		++_ref_count;
#endif
	}

	// const to allow shared_ptr<T const>
	void release() const
	{
#if GVL_THREADSAFE
		#error "Not finished"
		if(_ref_count == 1) // 1 means it has to become 0, nobody can increment it after this read
			_delete();
		else
		{
			// TODO: Implement CAS
			int read_ref_count;
			do
			{
				read_ref_count = _ref_count;
			}
			while(!cas(&_ref_count, read_ref_count, read_ref_count - 1));

			if(read_ref_count - 1 == 0)
			{
				_clear_weak_ptrs();
				_delete();
			}
		}
#else
		--_ref_count;
		if(_ref_count == 0)
		{
			_clear_weak_ptrs();
			_delete();
		}
#endif
	}

	void swap(shared& b)
	{
		std::swap(_ref_count, b._ref_count);
		std::swap(_first, b._first);
	}

	int ref_count() const
	{ return _ref_count; }

	virtual ~shared()
	{
	}

private:
	void _delete() const
	{
		delete this;
	}

	void _clear_weak_ptrs() const
	{
	}

	mutable int _ref_count; // You should be able to have shared_ptr<T const>
	weak_ptr_common* _first;
};

} // namespace gvl

#endif // UUID_5F7548A068F9485B6759368B5BAE8157

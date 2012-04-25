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

#ifndef UUID_C2F0561FF8D54EC1F56F98B82F02D1C0
#define UUID_C2F0561FF8D54EC1F56F98B82F02D1C0

struct gvl_weak_ptr;

struct gvl_shared
{
	shared()
	: _ref_count(0), _first(0)
	{

	}

	void add_ref()
	{
#if GVL_THREADSAFE
		#error "Not finished"
		// TODO: Interlocked add
		++_ref_count;
#else
		++_ref_count;
#endif
	}

	void release()
	{
#if GVL_THREADSAFE
		#error "Not finished"
		if(_ref_count == 1) // 1 means it has to become 0, nobody can increment it after this read
			_delete();
		else
		{
			// TODO: CAS decrement the counter and check
			cas(
		}
#else
		--_ref_count;
		if(_ref_count == 0)
		{
			_delete();
		}
#endif
	}

	int ref_count() const
	{ return _ref_count; }

	virtual ~shared()
	{
	}

	int _ref_count;
	gvl_weak_ptr* first;
};

#endif // UUID_C2F0561FF8D54EC1F56F98B82F02D1C0

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

#ifndef UUID_91DED073ABD04CDA1FAAC9AC7CF9BAAD
#define UUID_91DED073ABD04CDA1FAAC9AC7CF9BAAD

namespace gvl
{

template<typename T, typename Tag = void>
struct disjoint_set_member
{
	disjoint_set_member()
	: parent(0), rank(0)
	{
	}

	// Define this in T for unification logic.
	// 'this' will be the new representative of the set.
	template<typename Tag2>
	void on_union(T const&)
	{
		// Do nothing by default
	}

	// Define this in T for reset logic.
	// This will be called when 'this' is made into a singleton
	// set.
	template<typename Tag2>
	void on_reset()
	{
		// Do nothing by default
	}

	bool same_set(T& b)
	{
		return &find() == &b.find();
	}

	T& find()
	{
		// Path compression
		if(!parent)
			return *static_cast<T*>(this);

		parent = &parent->find();
		return *parent;
	}

	// Unify this and b and return the representative of the union
	T& union_(T& b)
	{
		// Union by rank
		T& rootA = find();
		T& rootB = b.find();

		sassert(!rootA.parent && !rootB.parent);

		if(&rootA == &rootB)
			return rootA;

		int rankA = rootA.rank;
		int rankB = rootB.rank;

		if(rankA < rankB)
		{
			rootA.parent = &rootB;
			rootB.on_union<Tag>(rootA);
			return rootB;
		}
		else
		{
			rootB.parent = &rootA;
			rootA.on_union<Tag>(rootB);
			if(rankA == rankB)
				rootA.rank = rankA + 1;
			return rootA;
		}
	}

	// Make this member into a singleton set.
	// This only has defined behaviour if all elements of the set are reset()
	// before any element in the set is used again.
	void reset()
	{
		parent = 0;
		rank = 0;
		derived().on_reset<Tag>();
	}

	T& derived()
	{ return *static_cast<T*>(this); }

	T* parent;
	int rank;
};

}

#endif // UUID_91DED073ABD04CDA1FAAC9AC7CF9BAAD

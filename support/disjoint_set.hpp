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

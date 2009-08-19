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
		
		if(&rootA == &rootB)
			return rootA;
		
		int rankA = rootA.rank;
		int rankB = rootB.rank;
		
		if(rankA < rankB)
		{
			rootA.parent = &rootB;
			return rootB;
		}
		else
		{
			rootB.parent = &rootA;
			if(rankA == rankB)
				++rootA.rank;
			return rootA;
		}
	}
	
	// Make this member into it's own set again.
	// This only has defined behaviour if all elements of the set are reset()
	// before any element in the set is used.
	void reset()
	{
		parent = 0;
		rank = 0;
	}
	
	T* parent;
	int rank;
};

}

#endif // UUID_91DED073ABD04CDA1FAAC9AC7CF9BAAD

#ifndef UUID_62728EE0B31F4930782885B97479EAC1
#define UUID_62728EE0B31F4930782885B97479EAC1

#include <climits>
#include <cstddef>
#include <algorithm>
#include "../support/functional.hpp"
#include "../support/debug.hpp"
#include "../math/tt800.hpp"
#include "../system/system.hpp"


namespace gvl
{


struct treap_node_common 
{
/*
	static std::size_t const left_bit = 2ul;
	static std::size_t const right_bit = 1ul;
*/
	treap_node_common* parent;
	treap_node_common* ch[2];
	std::size_t priority;
	
	template<int Ch>
	treap_node_common*& child()
	{
		return ch[Ch];
	}
	
	template<int Ch>
	treap_node_common*& mchild()
	{
		return ch[Ch^1];
	}
	
	template<int Ch>
	bool has_()
	{
		return !!ch[Ch];
	}
	
	template<int Ch> // Ch == 0 => left, Ch == 1 => right
	treap_node_common* move()
	{
		treap_node_common* n = ch[Ch];
		if(n)
			return n->extreme_in_subtree<Ch^1>();

		n = this;
		while(true)
		{
			if(!n->parent)
				return 0;
			if(n->parent->ch[Ch^1] == n) // Arrived from other direction, n->parent must be next
				return n->parent;
			n = n->parent;
		}
	}
	
	template<int Ch> // Ch == 0 => left, Ch == 1 => right
	treap_node_common* extreme_in_subtree()
	{
		treap_node_common* n = this;
		while(n->ch[Ch])
			n = n->ch[Ch];
		return n;
	}
		
	bool is_leaf() const
	{
		return !ch[0] && !ch[1];
	}
};

template<typename DerivedT>
struct treap_node : treap_node_common
{

};

struct default_random
{
	default_random()
	: r(get_ticks())
	{
	}
	
	std::size_t operator()()
	{
		return r();
	}
	
	tt800 r; // TODO: A smaller PRNG?
};

template<typename T
, typename Compare = std::less<T>
, typename Deleter = default_delete
, typename Random = default_random>
struct treap : Compare, Random, Deleter
{
	typedef T value_type;
	
	static T* downcast(treap_node_common* p)
	{
		return static_cast<T*>(static_cast<treap_node<T>*>(p));
	}

	static treap_node<T>* upcast(T* p)
	{
		return static_cast<treap_node<T>*>(p);
	}
	
	struct iterator
	{
		friend struct treap;
		
		typedef ptrdiff_t difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T* pointer;
		typedef T& reference;
		typedef T value_type;
    
		iterator()
		: ptr_(0)
		{}

/*
		explicit iterator(treap_node_common* ptr)
		: ptr_(ptr)
		{}
		*/
		explicit iterator(T* ptr)
		: ptr_(upcast(ptr))
		{}

		T& operator*()
		{ return *downcast(ptr_); }

		T* operator->()
		{ return downcast(ptr_); }

		iterator& operator++()
		{
			ptr_ = ptr_->move<1>();
			return *this;
		}

		iterator& operator--()
		{
			ptr_ = ptr_->move<0>();
			return *this;
		}
		
		iterator next() const
		{
			return iterator(ptr_->move<1>());
		}
		
		iterator prev() const
		{
			return iterator(ptr_->move<0>());
		}

		bool operator==(iterator const& b)
		{
			return b.ptr_ == ptr_;
		}

		bool operator!=(iterator const& b)
		{
			return b.ptr_ != ptr_;
		}
		
	private:
		treap_node_common* ptr_;
	};

	treap()
	: n(0)
	, root(0)
	{
	}
	
	~treap()
	{
		clear();
	}
	
	bool empty() const
	{
		return n == 0;
	}
	
	std::size_t size() const
	{
		return n;
	}
	
	void clear()
	{
		if(root)
			delete_node_(root);
		root = 0;
		n = 0;
	}
	
	iterator begin()
	{
		sassert(root);
		return root->extreme_in_subtree<0>();
	}
	
	iterator end()
	{
		return iterator(0);
	}
	
	void swap(treap& b)
	{
		std::swap(n, b.n);
		std::swap(root, b.root);
	}

	void insert(T* el_)
	{
		treap_node_common* el = upcast(el_);
		
		// Both right and left are initially null-threads
		el->priority = Random::operator()();
		el->ch[0] = 0; // TODO: Last element
		el->ch[1] = 0; // TODO: First element

		if(root)
			insert_under_(root, el, 0, root);
		else
			root = el;
		++n;
	}
	
	void unlink(T* el_)
	{
		treap_node_common* el = upcast(el_);
		if(el->parent)
			root_unlink_(el, el->parent,
				el->parent->child<0>() == el ?
					el->parent->child<0>() : el->parent->child<1>());
		else
			root_unlink_(el, 0, root);
			
		--n;
	}
	
	void erase(iterator i)
	{
		T* el_ = downcast(i.ptr_);
		unlink(el_);
		Deleter::operator()(el_);
	}
	
	template<typename SpecKeyT>
	iterator find(SpecKeyT const& k)
	{
		return iterator(downcast(find_(k, root)));
	}

private:

	template<typename SpecKeyT>
	treap_node_common* find_(SpecKeyT const& k, treap_node_common* el)
	{
		if(!el)
			return 0;
			
		if(Compare::operator()(k, *downcast(el)))
			return find_(k, el->child<0>());
		else if(Compare::operator()(*downcast(el), k))
			return find_(k, el->child<1>());
		else
			return el;
	}

	void delete_node_(treap_node_common* el)
	{
		if(el->has_<0>())
			delete_node_(el->child<0>());
		if(el->has_<1>())
			delete_node_(el->child<1>());
		Deleter::operator()(downcast(el));
	}
	
	template<int Ch>
	static void rotate_with_(
		treap_node_common* n,
		treap_node_common* parent,
		treap_node_common*& child_slot)
	{
		treap_node_common* k1 = n->child<Ch>();
		
		n->parent = k1;
		n->child<Ch>() = k1->mchild<Ch>();
		k1->parent = parent;
		k1->mchild<Ch>() = n;
		child_slot = k1;
	}
		
	template<int Ch>
	void insert_side_(
		treap_node_common* n,
		treap_node_common* el,
		treap_node_common* parent,
		treap_node_common*& child_slot)
	{
		insert_under_(n->child<Ch>(), el, n, n->child<Ch>());
		if(n->child<Ch>()->priority < n->priority)
			return rotate_with_<Ch>(n, parent, child_slot);
	}
	
	void insert_under_(
		treap_node_common* n,
		treap_node_common* el,
		treap_node_common* parent,
		treap_node_common*& child_slot)
	{
		if(!n)
		{
			child_slot = el;
			el->parent = parent;
		}
		else if(Compare::operator()(*downcast(el), *downcast(n)))
		{
			insert_side_<0>(n, el, parent, child_slot);
		}
		else if(Compare::operator()(*downcast(n), *downcast(el)))
		{
			insert_side_<1>(n, el, parent, child_slot);
		}
	}
	
	template<int Ch>
	void root_unlink_child_(
		treap_node_common* n,
		treap_node_common* parent,
		treap_node_common*& child_slot)
	{
		rotate_with_<Ch>(n, parent, child_slot);
		if(n->is_leaf())
		{
			child_slot = 0;
		}
		else
			root_unlink_(n, parent, child_slot);
	}
	
	void root_unlink_(
		treap_node_common* n,
		treap_node_common* parent,
		treap_node_common*& child_slot)
	{
		if(!n->has_<0>())
		{
			child_slot = n->child<1>();
		}
		else if(!n->has_<1>())
		{
			child_slot = n->child<0>();
		}
		else if(n->child<0>()->priority < n->child<1>()->priority)
		{
			root_unlink_child_<0>(n, parent, child_slot);
		}
		else
		{
			root_unlink_child_<1>(n, parent, child_slot);
		}
	}
	
#if 0 // Only useful without parent pointers
	void unlink_(
		treap_node_common* n,
		treap_node_common* el,
		treap_node_common* parent,
		treap_node_common*& child_slot)
	{
		if(Compare::operator()(*downcast(el), *downcast(n)))
		{
			if(n->child<0>())
				unlink_(n->child<0>(), el, n, n->child<0>());
		}
		else if(Compare::operator()(*downcast(n), *downcast(el)))
		{
			if(n->child<1>())
				unlink_(n->child<1>(), el, n, n->child<1>());
		}
		else
		{
			root_unlink_(n, parent, child_slot);
		}
	}
#endif
	
	
	void clean_()
	{
		n = 0;
		root = 0;
	}
	
	std::size_t n;
	treap_node_common* root;
};

template<typename KeyT, typename ValueT>
struct treap_map_node : treap_node<treap_map_node<KeyT, ValueT> >
{
	treap_map_node(KeyT const& first)
	: first(first)
	{
	}
	
	KeyT first;
	ValueT second;
};


template<typename KeyT, typename ValueT, typename Compare>
struct treap_map_compare : Compare
{
	bool operator()(
		treap_map_node<KeyT, ValueT> const& a,
		treap_map_node<KeyT, ValueT> const& b) const
	{
		return Compare::operator()(a.first, b.first);
	}
	
	bool operator()(
		KeyT const& a,
		treap_map_node<KeyT, ValueT> const& b) const
	{
		return Compare::operator()(a, b.first);
	}
	
	bool operator()(
		treap_map_node<KeyT, ValueT> const& a,
		KeyT const& b) const
	{
		return Compare::operator()(a.first, b);
	}
};

template<typename KeyT, typename ValueT
, typename Compare = std::less<KeyT>
, typename Deleter = default_delete
, typename Random = default_random>
struct treap_map
: treap<
	treap_map_node<KeyT, ValueT>,
	treap_map_compare<KeyT, ValueT, Compare>,
	Deleter,
	Random>
{
	typedef treap<
		treap_map_node<KeyT, ValueT>,
		treap_map_compare<KeyT, ValueT, Compare>,
		Deleter,
		Random> base;
	typedef treap_map_node<KeyT, ValueT> value_type;
	typedef typename base::iterator iterator;
	
	ValueT& operator[](KeyT const& k)
	{
		iterator el_ = this->find(k);
		if(el_ != this->end())
			return el_->second;
		
		value_type* p = new value_type(k);
		this->insert(p);
		return p->second;		
	}
};

} // namespace gvl

#endif // UUID_62728EE0B31F4930782885B97479EAC1

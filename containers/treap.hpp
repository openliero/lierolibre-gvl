#ifndef UUID_62728EE0B31F4930782885B97479EAC1
#define UUID_62728EE0B31F4930782885B97479EAC1

#include <climits>
#include <cstddef>
#include <algorithm>
#include "../support/functional.hpp"
#include "../support/debug.hpp"
#include "../math/tt800.hpp"
#include "../system/system.hpp"

#include <map>

namespace gvl
{

// NOTE: Not quite in working condition right now

struct treap_node_common 
{
	static int const left_bit = 1;
	static int const right_bit = 2;

	treap_node_common* parent;
	treap_node_common* ch[2];
	int thread_flags; // 
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
		return ((thread_flags >> Ch) & 1) != 0;
	}
	
	template<int Ch> // Ch == 0 => left, Ch == 1 => right
	treap_node_common* move()
	{
		if(!has_<Ch>())
			return ch[Ch]; // It's a thread
			
		return ch[Ch]->extreme_in_subtree<Ch^1>();
	}
	
	template<int Ch> // Ch == 0 => left, Ch == 1 => right
	treap_node_common* extreme_in_subtree()
	{
		treap_node_common* n = this;
		while(n->has_<Ch>())
			n = n->ch[Ch];
		return n;
	}
		
	bool is_leaf() const
	{
		return thread_flags == 0;
	}
	
};

struct default_treap_tag
{
};

template<typename Tag = default_treap_tag>
struct treap_node : treap_node_common
{

};

struct default_random
{
	default_random()
	: r(1) // TEMP r(get_ticks())
	{
	}
	
	std::size_t operator()()
	{
		return r();
	}
	
	tt800 r; // TODO: A smaller PRNG?
};

template<typename T
, typename Tag = default_treap_tag
, typename Compare = std::less<T>
, typename Deleter = default_delete
, typename Random = default_random>
struct treap : Compare, Random, Deleter
{
	typedef T value_type;
	
	static T* downcast(treap_node_common* p)
	{
		return static_cast<T*>(static_cast<treap_node<Tag>*>(p));
	}

	static treap_node<Tag>* upcast(T* p)
	{
		return static_cast<treap_node<Tag>*>(p);
	}
	
	struct range
	{
		friend struct treap;
		
		bool empty() const
		{
			return front_ == back_prev_;
		}
		
		T& front()
		{
			return *downcast(front_);
		}
		
		T& back()
		{
			return *downcast(back_);
		}
		
		void pop_front()
		{
			front_ = front_->move<1>();
		}
		
		void pop_back()
		{
			back_prev_ = back_;
			back_ = back_->move<0>();
		}
		
	private:
		range(treap_node_common* front_init, treap_node_common* back_init)
		: front_(front_init)
		, back_(back_init)
		, back_prev_(back_->move<1>())
		{
		}
		
		range(treap_node_common* front_init, treap_node_common* back_init, treap_node_common* back_prev_init)
		: front_(front_init)
		, back_(back_init)
		, back_prev_(back_prev_init)
		{
		}
		
		treap_node_common* front_;
		treap_node_common* back_;
		treap_node_common* back_prev_;
	};
	
#if 0
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
#endif

	treap()
	: n(0)
	, root(0)
	{
		head.thread_flags = 0; // Head is always a leave (sort of)
		head.ch[0] = &head;
		head.ch[1] = &head;
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
		clean_();
	}
	
	/*
	iterator begin()
	{
		sassert(root);
		return root->extreme_in_subtree<0>();
	}
	
	iterator end()
	{
		return iterator(0);
	}*/
	
	range all()
	{
		return range(head.child<1>(), head.child<0>(), &head);
	}
	
	void swap(treap& b)
	{
		std::swap(n, b.n);
		std::swap(root, b.root);
		
		std::swap(head.ch[0], b.head.ch[0]);
		std::swap(head.ch[1], b.head.ch[1]);
		// All head nodes have thread_flags == 0, no need to swap
	}

	void insert(T* el_)
	{
		treap_node_common* el = upcast(el_);
		
		el->priority = Random::operator()();
		el->thread_flags = 0; // Newly inserted nodes are always leaves

		if(root)
			insert_under_(root, el, 0, root, &head, &head);
		else
		{
			root = el;
			root->parent = 0;
			root->ch[0] = &head;
			root->ch[1] = &head;
			head.ch[0] = root;
			head.ch[1] = root;
			sassert(n == 0);
			n = 1;
		}
	}
	
	void unlink(T* el_)
	{
		treap_node_common* el = upcast(el_);
		
		treap_node_common* parent = el->parent;
		
		if(parent)
		{
			if(parent->child<0>() == el)
				unlink_nonroot_<0>(el, parent);
			else
				unlink_nonroot_<1>(el, parent);
		}
		else
		{
			if(el->is_leaf())
			{
				root = 0;
				head.ch[0] = &head;
				head.ch[1] = &head;
			}
			else
			{
				if(el == head.ch[0])
					head.ch[0] = el->move<1>();
				if(el == head.ch[1])
					head.ch[1] = el->move<0>();
					
				root_unlink_(el, 0, root);
			}
		}
		
		--n;
	}
	
	T* unlink_front(range& r)
	{
		sassert(!r.empty());
		
		T* front = &r.front();
		r.pop_front();
		unlink(front);
		return front;
	}
	
	T* unlink_back(range& r)
	{
		sassert(!r.empty());
		
		T* back = &r.front();
		// We don't want to call pop_back() as it would set back_prev_ to the unlinked element.
		// back_prev_ remains correct.
		r.back_ = r.back_->move<0>();
		unlink(back);
		return back;
	}
	
	
	/*
	void erase(iterator i)
	{
		T* el_ = downcast(i.ptr_);
		unlink(el_);
		Deleter::operator()(el_);
	}*/
	
	void erase_front(range& r)
	{
		Deleter::operator()(unlink_front(r));
	}
	
	void erase_back(range& r)
	{
		Deleter::operator()(unlink_back(r));
	}
	
	template<typename SpecKeyT>
	range find(SpecKeyT const& k)
	{
		return range(downcast(find_(k, root)), 0);
	}
	
	template<typename SpecKeyT>
	bool test(SpecKeyT const& k)
	{
		return find(k) != end();
	}
	
	// Assumes the predicate is true for elements [0, x) and false for [x, count) where x is in [0, count]
	template<typename SpecKeyT, typename Relation>
	treap_node_common* last_where(SpecKeyT const& k, Relation rel)
	{
		treap_node_common* prev = 0;
		
		if(!root)
			return &head;
		treap_node_common* el = root;
		
		while(true)
		{
			if(pred(*downcast(el), k))
			{
				// Left and center satisfy the condition, we can skip left completely.
				// prev is the last known qualifying element.
				prev = el;
				if(!el->has_<1>())
					return prev;
				el = el->child<1>();
			}
			else
			{
				// Center and right doesn't satisfy, but left may
				if(!el->has_<0>())
					return prev;
				el = el->child<0>();
			}
		}
	}
	
	// Assumes the predicate is true for elements [x, count) and false for [0, x) where x is in [0, count]
	template<typename SpecKeyT, typename Relation>
	treap_node_common* first_where(SpecKeyT const& k, Relation rel)
	{
		treap_node_common* prev = 0;
		
		if(!root)
			return &head;
		treap_node_common* el = root;
		
		while(true)
		{
			if(pred(*downcast(el), k))
			{
				// Right and center satisfy the condition, we can skip right completely.
				// prev is the last known qualifying element.
				prev = el;
				if(!el->has_<0>())
					return prev;
				el = el->child<0>();
			}
			else
			{
				// Center and left doesn't satisfy, but right may
				if(!el->has_<1>())
					return prev;
				el = el->child<1>();
			}
		}
	}
	
	void integrity_check()
	{
		range r = all();
		
		std::size_t count = 0;
		
		if(!r.empty())
		{
			++count;
			
			passert(root->extreme_in_subtree<0>() == head.ch[1], "Right of head should be the smallest element");
			passert(root->extreme_in_subtree<1>() == head.ch[0], "Left of head should be the largest element");
				
			T* prev = &r.front();
			r.pop_front();
			check_node(prev);
			
			for(; !r.empty(); r.pop_front())
			{
				++count;
				
				passert(Compare::operator()(*prev, r.front()), "Each element must be larger than the previous");
				prev = &r.front();
				check_node(prev);
			}
		}
		
		passert(size() == count, "Manual count does not correspond to cached count");
	}
	
	std::size_t depth() const
	{
		if(!root)
			return 0;
		return depth_(root);
	}

private:

	std::size_t depth_(treap_node_common* node) const
	{
		std::size_t depth = 0;
		if(node->has_<0>())
			depth = std::max(depth, depth_(node->child<0>()));
		if(node->has_<1>())
			depth = std::max(depth, depth_(node->child<1>()));
		return 1 + depth;
	}

	void check_node(T* n_)
	{
		treap_node_common* n = upcast(n_);
		
		if(n->parent)
		{
			passert(n->parent->priority <= n->priority, "Parent priority must be less or equal to it's children's");
		}
		else
		{
			passert(n == root, "Only the root node can lack a parent");
		}
	}
	
	template<int Ch>
	void unlink_nonroot_(treap_node_common* el, treap_node_common* parent)
	{
		treap_node_common** child_slot = &parent->child<Ch>();
		
		// Correct head children if necessary
		if(el == head.mchild<Ch>()) // If we descended left, right of head could be an extreme and vice versa
			head.mchild<Ch>() = el->move<Ch^1>();
			
		if(el->is_leaf())
		{
			// If it's a leaf, the node on the side is the next one
			*child_slot = el->child<Ch>();
			passert(((parent->thread_flags >> Ch) & 1) != 0, "Parent must have the thread flag for this path set");
			parent->thread_flags ^= 1<<Ch; // Thread flag is set, we use XOR to unset it
		}
		else
		{
			// Unlink root of subtree
			root_unlink_(el, parent, *child_slot);
		}
	}

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
		
		
		// Copy the threading flag for k1->mchild<Ch>() to n->child<Ch>()
		/*
		int kmch = (k1->thread_flags & (2 >> Ch));
		int mirrored_kmch = (Ch == 0) ? (kmch >> 1) : (kmch << 1);
		n->thread_flags = (n->thread_flags & (2 >> Ch)) | mirrored_kmch;
		*/
		passert(n->thread_flags & (1 << Ch), "n->child<Ch>() flag must be set");
		if(k1->thread_flags & (2 >> Ch))
		{
			// n->child<Ch>() flag already set (k1 is a child)
			n->child<Ch>() = k1->mchild<Ch>();
		}
		else
		{
			n->thread_flags ^= (1 << Ch); // Clear n->child<Ch>() flag
			n->child<Ch>() = k1; // k1 is the new thread
		}
		
		k1->parent = parent;
		k1->mchild<Ch>() = n;
		k1->thread_flags |= (2 >> Ch); // Set threading flag for k1->mchild<Ch>()
		child_slot = k1;
	}
		
	template<int Ch>
	void insert_side_(
		treap_node_common* n,
		treap_node_common* el,
		treap_node_common* parent,
		treap_node_common*& child_slot,
		treap_node_common* prev_element, // The closest element smaller than all elements under the child
		treap_node_common* next_element) // The closest element smaller than all elements under the child
	{
		treap_node_common*& chr = n->child<Ch>();
		
		if(!n->has_<Ch>())
		{
			// If the previous or next element (depending on side) is the head,
			// the new element is a new extreme.
			if((Ch == 0 && prev_element == &head) || (Ch == 1 && next_element == &head))
				head.mchild<Ch>() = el; // New extreme node
				
			n->child<Ch>() = el;
			n->thread_flags |= 1<<Ch;
			
			el->parent = n;
			el->ch[0] = prev_element;
			el->ch[1] = next_element;
			++this->n;
		}
		else
		{
			insert_under_(chr, el, n, chr, prev_element, next_element);
		}
		
#if 1
		// NOTE: insert_under_ may change n->child<Ch>(), we must reread it here.
		if(chr->priority < n->priority)
			return rotate_with_<Ch>(n, parent, child_slot);
#endif
	}
	
	void insert_under_(
		treap_node_common* n,
		treap_node_common* el,
		treap_node_common* parent,
		treap_node_common*& child_slot,
		treap_node_common* prev_element, // The closest element smaller than all elements under n
		treap_node_common* next_element) // The closest element larger than all elements under n
	{
		sassert(n);
		
		if(Compare::operator()(*downcast(el), *downcast(n)))
		{
			insert_side_<0>(n, el, parent, child_slot, prev_element, n);
		}
		else if(Compare::operator()(*downcast(n), *downcast(el)))
		{
			insert_side_<1>(n, el, parent, child_slot, n, next_element);
		}
		else
		{
			// TODO: Equal to an existing element, throw or return something here?
			Deleter::operator()(downcast(el));
		}
	}
	
	/*
	void insert_under2_(
		treap_node_common* n,
		treap_node_common* el,
		treap_node_common* parent,
		treap_node_common*& child_slot,
		treap_node_common* prev_element, // The closest element smaller than all elements under n
		treap_node_common* next_element) // The closest element larger than all elements under n
	{
		sassert(n);
		
		if(Compare::operator()(*downcast(el), *downcast(n)))
		{
			insert_side_<0>(n, el, parent, child_slot, prev_element, n);
		}
		else if(Compare::operator()(*downcast(n), *downcast(el)))
		{
			insert_side_<1>(n, el, parent, child_slot, n, next_element);
		}
		else
		{
			// TODO: Equal to an existing element, throw or return something here?
			Deleter::operator()(downcast(el));
		}
	}*/
	
	template<int Ch>
	void root_unlink_child_(
		treap_node_common* n,
		treap_node_common* parent,
		treap_node_common*& child_slot)
	{
		passert(!n->is_leaf(), "Assumed to be a non-leaf");
		
		treap_node_common* new_parent = n->child<Ch>();
		
		rotate_with_<Ch>(n, parent, child_slot);
		
		passert(!n->is_leaf(), "Rotation cannot turn a non-leaf into a leaf");

		root_unlink_(n, new_parent, new_parent->mchild<Ch>());
	}
	
	void root_unlink_(
		treap_node_common* n,
		treap_node_common* parent,
		treap_node_common*& child_slot,
		int child_slot_mask)
	{
		assert(!n->is_leaf()); // Leafs should be handled before calling this function
		
		if(!n->has_<0>())
		{
			// TODO: Need to correct the threading flags of child_slot
			if(parent)
			{
				passert((parent->thread_flags & child_slot_mask) != 0, "Thread flag for child_slot must be set");
				int right_mask = -(n->thread_flags >> 1); // All 1 if right is a child, 0 otherwise
				
				// Mask child_slot thread flag if there is no child
				parent->thread_flags = parent->thread_flags & (right_mask & child_slot_mask);
			}
			child_slot = n->child<1>();
		}
		else if(!n->has_<1>())
		{
			// As the test above failed, we know that there's a child to the left,
			// we don't need to update the child_slot thread flag.
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
		head.ch[0] = &head;
		head.ch[1] = &head;
	}
	
	
	
	std::size_t n;
	treap_node_common* root;
	treap_node_common  head;
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

template<typename KeyT>
struct treap_set_node : treap_node<treap_set_node<KeyT> >
{
	treap_set_node(KeyT const& first)
	: value(value)
	{
	}
	
	KeyT value;
};


template<typename KeyT, typename Compare>
struct treap_set_compare : Compare
{
	bool operator()(
		treap_set_node<KeyT> const& a,
		treap_set_node<KeyT> const& b) const
	{
		return Compare::operator()(a.value, b.value);
	}
	
	bool operator()(
		KeyT const& a,
		treap_set_node<KeyT> const& b) const
	{
		return Compare::operator()(a, b.value);
	}
	
	bool operator()(
		treap_set_node<KeyT> const& a,
		KeyT const& b) const
	{
		return Compare::operator()(a.value, b);
	}
};

template<typename KeyT
, typename Compare = std::less<KeyT>
, typename Deleter = default_delete
, typename Random = default_random>
struct treap_set
: treap<
	treap_set_node<KeyT>,
	treap_set_compare<KeyT, Compare>,
	Deleter,
	Random>
{
	typedef treap<
		treap_set_node<KeyT>,
		treap_set_compare<KeyT, Compare>,
		Deleter,
		Random> base;
	typedef treap_set_node<KeyT> value_type;
	typedef typename base::iterator iterator;
};

} // namespace gvl

#endif // UUID_62728EE0B31F4930782885B97479EAC1

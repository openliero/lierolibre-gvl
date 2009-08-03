#ifndef UUID_72728EE0B31F4930782885B97479EAC2
#define UUID_72728EE0B31F4930782885B97479EAC2

#include <climits>
#include <cstddef>
#include <algorithm>
#include "../support/functional.hpp"
#include "../support/debug.hpp"
//#include "../math/tt800.hpp"
#include "../math/cmwc.hpp"
#include "../system/system.hpp"
#include <memory>

#include <map>

namespace gvl
{

// NOTE: Not quite in working condition right now
struct treap_node_common2 
{
	static int const left_bit = 1;
	static int const right_bit = 2;

	treap_node_common2* ch[2];
	treap_node_common2* parent;
	//int isnil;
#if 1
	std::size_t priority_;

	std::size_t priority() const
	{
		return priority_;
	}
#else
	std::size_t priority() const
	{
		std::size_t x = (std::size_t)this;

		x ^= x >> 7;
		x += 2654435761;
		x ^= x >> 17;
		return x;
	}
#endif
	
	template<int Ch>
	treap_node_common2*& child()
	{
		return ch[Ch];
	}
	
	template<int Ch>
	treap_node_common2*& mchild()
	{
		return ch[Ch^1];
	}
	
	template<int Ch>
	bool has_() const
	{
		return !ch[Ch]->is_nil();
	}
	
	bool is_nil() const
	{
		return parent == this; // Only head has itself as parent
	}
	
	template<int Ch> // Ch == 0 => left, Ch == 1 => right
	treap_node_common2* move()
	{
		treap_node_common2* next_ch = ch[Ch];
		if(!next_ch->is_nil())
			return next_ch->extreme_in_subtree<Ch^1>();

		treap_node_common2* lparent = parent;
		treap_node_common2* child = this;
			
		while(!lparent->is_nil() && lparent->ch[Ch] == child)
		{
			// Went up this path
			child = lparent;
			lparent = child->parent;
		}
		
		return lparent;
	}
	
	template<int Ch> // Ch == 0 => left, Ch == 1 => right
	treap_node_common2* extreme_in_subtree()
	{
		treap_node_common2* n = this;
		
		while(true)
		{
			treap_node_common2* child = n->ch[Ch];
			if(child->is_nil())
				return n;
			n = child;
		}
	}
		
	bool is_leaf() const
	{
		return !has_<0>() && !has_<1>();
	}
	
};

struct default_treap_tag2
{
};

template<typename Tag = default_treap_tag2>
struct treap_node2 : treap_node_common2
{

};

struct default_random2
{
	default_random2()
	: r(1) // TEMP r(get_ticks())
	{
	}
	
	std::size_t operator()()
	{
		return r();
	}
	
	//tt800 r; // TODO: A smaller PRNG?
	mwc r;
};

template<typename T
, typename Tag = default_treap_tag2
, typename Compare = std::less<T>
, typename Deleter = default_delete
, typename Random = default_random2>
struct treap2 : Compare, Random, Deleter
{
	typedef T value_type;
	
	static T* downcast(treap_node_common2* p)
	{
		return static_cast<T*>(static_cast<treap_node2<Tag>*>(p));
	}

	static treap_node2<Tag>* upcast(T* p)
	{
		return static_cast<treap_node2<Tag>*>(p);
	}
	
	struct range
	{
		friend struct treap2;
		
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
		range(treap_node_common2* front_init, treap_node_common2* back_init)
		: front_(front_init)
		, back_(back_init)
		, back_prev_(back_->move<1>())
		{
		}
		
		range(treap_node_common2* front_init, treap_node_common2* back_init, treap_node_common2* back_prev_init)
		: front_(front_init)
		, back_(back_init)
		, back_prev_(back_prev_init)
		{
		}
		
		treap_node_common2* front_;
		treap_node_common2* back_;
		treap_node_common2* back_prev_;
	};
	
#if 0
	struct iterator
	{
		friend struct treap2;
		
		typedef ptrdiff_t difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef T* pointer;
		typedef T& reference;
		typedef T value_type;
    
		iterator()
		: ptr_(0)
		{}

/*
		explicit iterator(treap_node_common2* ptr)
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
		treap_node_common2* ptr_;
	};
#endif

	treap2()
	: n(0)
	, head(new treap_node_common2)
	{
		head->ch[0] = head;
		head->ch[1] = head;
		head->parent = head;
		//head->isnil = true;
		head->priority_ = 0; // Head always has lower (or equal) priority to all others
		rotations = 0;
	}
	
	~treap2()
	{
		clear();
		delete head;
	}
	
	treap_node_common2*& root()
	{ return head->ch[0]; }
	
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
		if(root() != head)
			delete_node_(root());
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
		return range(head->extreme_in_subtree<0>(), head->extreme_in_subtree<1>(), head);
	}
	
	void swap(treap2& b)
	{
		std::swap(n, b.n);
		std::swap(head, b.head);
	}

	void insert(T* el_)
	{
		treap_node_common2* el = upcast(el_);
		
		std::size_t el_priority = Random::operator()();


		el->priority_ = el_priority;
		el->ch[0] = head;
		el->ch[1] = head;

		treap_node_common2* parent = head;
		treap_node_common2** child_slot = &parent->ch[0];

		while(true)
		{
			treap_node_common2* child = *child_slot;
			
			if(child == head)
			{
				*child_slot = el;
				el->parent = parent;
				++n;
				break;
			}
			
			if(Compare::operator()(*downcast(el), *downcast(child)))
			{
				child_slot = &child->ch[0];
			}
			else if(Compare::operator()(*downcast(child), *downcast(el)))
			{
				child_slot = &child->ch[1];
			}
			else
			{
				// TODO: Equal to an existing element, throw or return something here?
				Deleter::operator()(downcast(el));
				return;
			}
			
			parent = child;
		}
		
		// Fix up heap property

		treap_node_common2* lhead = head;

		if(el_priority < parent->priority())
		{
			do
			{
				++rotations;
				treap_node_common2* parent_parent = parent->parent;
				
				// If parent_parent is head, ch[0] will be picked
				treap_node_common2** parent_child_slot;

				if(parent_parent->ch[0] == parent)
				{
					parent_child_slot = &parent_parent->ch[0];
				}
				else
				{
					parent_child_slot = &parent_parent->ch[1];
				}

				if(child_slot == &parent->ch[0])
					rotate_with_<0>(parent, parent_parent, *parent_child_slot);
				else
					rotate_with_<1>(parent, parent_parent, *parent_child_slot);

				parent = parent_parent;
				child_slot = parent_child_slot;
			}
			while(el_priority < parent->priority());
		}
		
		head->ch[1] = root(); // Synchronize right branch in case it changed
	}
	
	void unlink(T* el_)
	{
		treap_node_common2* el = upcast(el_);
		treap_node_common2* parent = el->parent;
		
		if(parent->ch[0] == el)
		{
			// If parent is head, this branch will be taken. We correct
			// ch[1] because ch[0] was passed.
			root_unlink_(el, parent, &parent->ch[0]);
			head->ch[1] = root(); // Synchronize right branch in case it changed
		}
		else
			root_unlink_(el, parent, &parent->ch[1]);

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
		return range(downcast(find_(k, root())), head/*head->extreme_in_subtree<1>()*/, head);
	}
	
	template<typename SpecKeyT>
	bool test(SpecKeyT const& k)
	{
		return find(k) != end();
	}
	
	// Assumes the predicate is true for elements [0, x) and false for [x, count) where x is in [0, count]
	template<typename SpecKeyT, typename Relation>
	treap_node_common2* last_where(SpecKeyT const& k, Relation rel)
	{
		treap_node_common2* prev = 0;
		
		if(empty())
			return head;
		treap_node_common2* el = root();
		
		while(true)
		{
			if(pred(*downcast(el), k))
			{
				// Left and center satisfy the condition, we can skip left completely.
				// prev is the last known qualifying element.
				prev = el;
				el = el->child<1>();
				if(el == head)
					return prev;
			}
			else
			{
				// Center and right doesn't satisfy, but left may
				el = el->child<0>();
				if(el == head)
					return prev;
			}
		}
	}
	
	// Assumes the predicate is true for elements [x, count) and false for [0, x) where x is in [0, count]
	template<typename SpecKeyT, typename Relation>
	treap_node_common2* first_where(SpecKeyT const& k, Relation rel)
	{
		treap_node_common2* prev = 0;
		
		if(empty())
			return head;
		treap_node_common2* el = root();
		
		while(true)
		{
			if(pred(*downcast(el), k))
			{
				// Right and center satisfy the condition, we can skip right completely.
				// prev is the last known qualifying element.
				prev = el;
				el = el->child<0>();
				if(el == head)
					return prev;
			}
			else
			{
				// Center and left doesn't satisfy, but right may
				el = el->child<1>();
				if(el == head)
					return prev;
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
		if(empty())
			return 0;
		return depth_(head->ch[0]);
	}
	
	double average_depth() const
	{
		if(empty())
			return 0.0;
		return total_depth_(head->ch[0], 1.0) / size();
	}
	
	__int64 rotations;

private:

	void split_at_(treap_node_common2* tree, treap_node_common2* el)
	{
		treap_node_common2* arrow = el;
		int arrow_dir;
		treap_node_common2* split = tree;
		int split_dir;
		
		if(Compare::operator()(*downcast(el), *downcast(tree)))
		{
			el->ch[1] = tree;
			tree->parent = el;
			arrow_dir = 0;
			split_dir = 0;
		}
		else if(Compare::operator()(*downcast(tree), *downcast(el)))
		{
			el->ch[0] = tree;
			tree->parent = el;
			arrow_dir = 1;
			split_dir = 1;
		}
		else
		{
			// TODO: Equal to an existing element, throw or return something here?
			Deleter::operator()(downcast(el));
			return;
		}
		
		while(true)
		{
			treap_node_common2* split_node = split->ch[split_dir];
			
			if(split_node == head)
			{
				arrow->ch[arrow_dir] = head; // Close off arrow
				return;
			}
			
			if(Compare::operator()(*downcast(el), *downcast(split_node)))
			{
				if(split_dir == 0)
				{
					// We don't need to split, test next
					split = split->ch[split_dir];
				}
				else
				{
					// Need to split
					arrow->ch[arrow_dir] = split_node;
					split->parent = arrow;
					arrow = split;
					split = split_node;
					split_dir ^= 1;
					arrow_dir ^= 1;
				}
			}
			else if(Compare::operator()(*downcast(split_node), *downcast(el)))
			{
				if(split_dir == 1)
				{
					// We don't need to split, test next
					split = split->ch[split_dir];
				}
				else
				{
					// Need to split
					arrow->ch[arrow_dir] = split_node;
					split->parent = arrow;
					arrow = split;
					split = split_node;
					split_dir ^= 1;
					arrow_dir ^= 1;
				}
			}
			else
			{
				// Problem! We'd have to rotate up this node to join the two created trees.
				// If we could replace the old node, we would delete it and continue inserting the new
				// one.
				// TODO: Equal to an existing element, throw or return something here?
				Deleter::operator()(downcast(el));
				return;
			}
		}
	}

	std::size_t depth_(treap_node_common2 const* node) const
	{
		std::size_t depth = 0;
		if(node->has_<0>())
			depth = std::max(depth, depth_(node->ch[0]));
		if(node->has_<1>())
			depth = std::max(depth, depth_(node->ch[1]));
		return 1 + depth;
	}
	
	double total_depth_(treap_node_common2 const* node, double depth) const
	{
		double depth_sum = depth;
		if(node->has_<0>())
			depth_sum += total_depth_(node->ch[0], depth + 1.0);
		if(node->has_<1>())
			depth_sum += total_depth_(node->ch[1], depth + 1.0);
		return depth_sum;
	}

	void check_node(T* n_)
	{
		treap_node_common2* n = upcast(n_);
		
		if(n->parent != head)
		{
			passert(n->parent->priority() <= n->priority(), "Parent priority must be less or equal to it's children's");
		}
		else
		{
			passert(n == root(), "Only the root node can have the head as parent");
			passert(n == head->ch[1], "Both head branches must point to the root");
		}
	}
	
	template<typename SpecKeyT>
	treap_node_common2* find_(SpecKeyT const& k, treap_node_common2* el)
	{
		while(el != head)
		{
			if(Compare::operator()(k, *downcast(el)))
				el = el->child<0>();
			else if(Compare::operator()(*downcast(el), k))
				el = el->child<1>();
			else
				return el;
		}
		
		return el;
	}
	
	void delete_node_(treap_node_common2* el)
	{
		if(el->has_<0>())
			delete_node_(el->child<0>());
		if(el->has_<1>())
			delete_node_(el->child<1>());
		Deleter::operator()(downcast(el));
	}
	
	template<int Ch>
	void rotate_with_(
		treap_node_common2* n,
		treap_node_common2* parent,
		treap_node_common2*& child_slot)
	{
		treap_node_common2*& nchr = n->child<Ch>();
		treap_node_common2* k1 = nchr;
		treap_node_common2*& kmchr = k1->mchild<Ch>();
		treap_node_common2* kmch = kmchr;
		
		// k1 <-> n connection
		n->parent = k1;
		kmchr = n;
		
		// n <-> prev k1 child connection
		nchr = kmch;
		if(kmch != head) // kmch could be head, and we don't want to mess up parent of head (must be itself)
			kmch->parent = n;
		
		// k1 <-> parent connection
		k1->parent = parent;
		child_slot = k1;
	}

#if 0
	template<int Ch>
	void root_unlink_child_(
		treap_node_common2* n,
		treap_node_common2* parent,
		treap_node_common2*& child_slot)
	{
		passert(!n->is_leaf(), "Assumed to be a non-leaf");
		
		treap_node_common2* new_parent = n->child<Ch>();
		
		rotate_with_<Ch>(n, parent, child_slot);
		
		passert(!n->is_leaf(), "Rotation cannot turn a non-leaf into a leaf");

		root_unlink_(n, new_parent, new_parent->mchild<Ch>());
	}
#endif
	
	void root_unlink_(treap_node_common2* n, treap_node_common2* parent, treap_node_common2** child_slot)
	{
		treap_node_common2* lhead = head;
		while(true)
		{
			treap_node_common2* ch0 = n->ch[0];
			treap_node_common2* ch1 = n->ch[1];
			if(ch0 == lhead)
			{
				*child_slot = ch1;
				if(ch1 != lhead) // Do not disturb parent of head
					ch1->parent = parent;
				return;
			}
			else if(ch1 == lhead)
			{
				*child_slot = ch0;
				ch0->parent = parent; // We know ch0 is not head here
				return;
			}
			else if(ch0->priority() <= ch1->priority())
			{
				treap_node_common2* new_parent = ch0;
				rotate_with_<0>(n, parent, *child_slot);
				
				parent = new_parent;
				child_slot = &new_parent->ch[1];
			}
			else
			{
				treap_node_common2* new_parent = ch1;
				rotate_with_<1>(n, parent, *child_slot);
				
				parent = new_parent;
				child_slot = &new_parent->ch[0];
			}
		}
	}
	
#if 0
	void root_unlink_(
		treap_node_common2* n,
		treap_node_common2* parent,
		treap_node_common2*& child_slot,
		int child_slot_mask)
	{
		assert(!n->is_leaf()); // Leafs should be handled before calling this function
		
		
		
		if(n->child<0>() == head)
		{
			child_slot = n->child<1>();
		}
		else if(n->child<1>() == head)
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
#endif
	
#if 0 // Only useful without parent pointers
	void unlink_(
		treap_node_common2* n,
		treap_node_common2* el,
		treap_node_common2* parent,
		treap_node_common2*& child_slot)
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
		head->ch[0] = head;
		head->ch[1] = head;
		sassert(head->parent == head);
	}
	
	
	
	std::size_t n;
	treap_node_common2* head;
};

template<typename KeyT, typename ValueT>
struct treap_map_node2 : treap_node2<treap_map_node2<KeyT, ValueT> >
{
	treap_map_node2(KeyT const& first)
	: first(first)
	{
	}
	
	KeyT first;
	ValueT second;
};


template<typename KeyT, typename ValueT, typename Compare>
struct treap_map_compare2 : Compare
{
	bool operator()(
		treap_map_node2<KeyT, ValueT> const& a,
		treap_map_node2<KeyT, ValueT> const& b) const
	{
		return Compare::operator()(a.first, b.first);
	}
	
	bool operator()(
		KeyT const& a,
		treap_map_node2<KeyT, ValueT> const& b) const
	{
		return Compare::operator()(a, b.first);
	}
	
	bool operator()(
		treap_map_node2<KeyT, ValueT> const& a,
		KeyT const& b) const
	{
		return Compare::operator()(a.first, b);
	}
};

template<typename KeyT, typename ValueT
, typename Compare = std::less<KeyT>
, typename Deleter = default_delete
, typename Random = default_random2>
struct treap_map2
: treap2<
	treap_map_node2<KeyT, ValueT>,
	treap_map_compare2<KeyT, ValueT, Compare>,
	Deleter,
	Random>
{
	typedef treap2<
		treap_map_node2<KeyT, ValueT>,
		treap_map_compare2<KeyT, ValueT, Compare>,
		Deleter,
		Random> base;
	typedef treap_map_node2<KeyT, ValueT> value_type;
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
struct treap_set_node2 : treap_node2<treap_set_node2<KeyT> >
{
	treap_set_node2(KeyT const& first)
	: value(value)
	{
	}
	
	KeyT value;
};


template<typename KeyT, typename Compare>
struct treap_set_compare2 : Compare
{
	bool operator()(
		treap_set_node2<KeyT> const& a,
		treap_set_node2<KeyT> const& b) const
	{
		return Compare::operator()(a.value, b.value);
	}
	
	bool operator()(
		KeyT const& a,
		treap_set_node2<KeyT> const& b) const
	{
		return Compare::operator()(a, b.value);
	}
	
	bool operator()(
		treap_set_node2<KeyT> const& a,
		KeyT const& b) const
	{
		return Compare::operator()(a.value, b);
	}
};

template<typename KeyT
, typename Compare = std::less<KeyT>
, typename Deleter = default_delete
, typename Random = default_random2>
struct treap_set2
: treap2<
	treap_set_node2<KeyT>,
	treap_set_compare2<KeyT, Compare>,
	Deleter,
	Random>
{
	typedef treap2<
		treap_set_node2<KeyT>,
		treap_set_compare2<KeyT, Compare>,
		Deleter,
		Random> base;
	typedef treap_set_node2<KeyT> value_type;
	typedef typename base::iterator iterator;
};

} // namespace gvl

#endif // UUID_72728EE0B31F4930782885B97479EAC2

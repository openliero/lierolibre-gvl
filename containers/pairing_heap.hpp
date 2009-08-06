#ifndef UUID_332507506BA94073DD795F88B83DFD8B
#define UUID_332507506BA94073DD795F88B83DFD8B

#include <cstddef>
#include <algorithm>
#include <functional>
#include "../support/functional.hpp"
#include "../support/debug.hpp"

namespace gvl
{

struct pairing_node_common
{
	pairing_node_common** prev_next;    // Previous node slot pointing to this
	pairing_node_common* left_child;    // Left child
	pairing_node_common* right_sibling; // Right sibling
};

struct default_pairing_tag;

template<typename TagT = default_pairing_tag>
struct pairing_node : pairing_node_common
{

};

template<typename T, typename TagT = default_pairing_tag, typename Compare = std::less<T>, typename Deleter = default_delete>
struct pairing_heap : Compare, Deleter
{
	typedef pairing_node<TagT> node_t;
	
	static T* downcast(pairing_node_common* p)
	{
		return static_cast<T*>(static_cast<node_t*>(p));
	}

	static node_t* upcast(T* p)
	{
		return static_cast<node_t*>(p);
	}
	
	pairing_heap()
	: root(0)
	//, n(0)
	{
	}
	
	~pairing_heap()
	{
		clear();
	}
	
	bool empty() const
	{
		return !root;
	}
		
	void swap(pairing_heap& b)
	{
		std::swap(root, b.root);
		//std::swap(n, b.n);
	}
	
	void meld(pairing_heap& b)
	{
		if(root && b.root) // comparison_link_ assumes non-zero pointers
		{
			root = comparison_link_(root, b.root);
		}
		else if(!root)
		{
			// root is 0, but b.root may not be
			root = b.root;
		}
		// If root is non-zero and b.root is zero, we leave root as is
		
		b.root = 0;
	}
	
	// NOTE: TODO: Does root->prev have to have a defined value?
	
	void insert(T* el_)
	{
		pairing_node_common* el = upcast(el_);
		
		el->left_child = 0;
		//el->right_sibling = 0; // TODO: NOTE: Do we need to do this?
		//el->prev = 0; // TODO: NOTE: Do we need to do this?
		
		if(!root)
		{
			root = el;
		}
		else
		{
			root = comparison_link_(root, el);
		}
		//++n;
	}
	
	// NOTE: This actually "works" for increasing as well,
	// but this fact may change! Certainly the runtime
	// complexity bounds do not hold if keys are increased
	// using this function.
	void decreased_key(T* el_)
	{
		pairing_node_common* el = upcast(el_);
		
		if(el != root)
		{
			// NOTE: If prev is still larger than el, we can quit now, because:
			// * If prev is the parent, then obviously the heap property still holds.
			// * If prev is a sibling, then the parent must be larger than the sibling, and the heap property holds.
			// Does this affect the complexity in a negative way? One wouldn't think so.
			// A better question is whether it's common enough to pay off.
			
			// Assuming the children are in no particular order, the probability that
			// we can skip the tree manipulation ought to be less than 50%.
			
			// Assuming 50% probability, this means that the expected average
			// number of comparisons done per decreased_key would be 1.5 compared to
			// 1 without the optimization.
						
			unlink_subtree_(el);
				
			root = comparison_link_(root, el);
			//root->right_sibling = 0; // TODO: NOTE: Do we need to do this?
			//root->prev = 0; // TODO: NOTE: Do we need to do this?
		}
	}
	
	T* unlink_min()
	{
		passert(root, "Empty heap");
		
		pairing_node_common* ret = root;
		
		pairing_node_common* left_child = ret->left_child;
		if(left_child)
		{
			root = combine_siblings_(left_child);
			//root->right_sibling = 0; // TODO: NOTE: Do we need to do this?
			//root->prev = 0; // TODO: NOTE: Do we need to do this?
		}
		else
			root = 0;
		return downcast(ret);
	}
	
	
	T* unlink(T* el_)
	{
		pairing_node_common* el = upcast(el_);
		
		if(el != root)
		{
			unlink_subtree_(el);
				
			if(el->left_child)
			{
				root = comparison_link_(root, combine_siblings_(el->left_child));
			}
		}
		else
			unlink_min();
			
		return el_;
	}
	
	void erase_min()
	{
		passert(root, "Empty heap");
		pairing_node_common* old = root;
		unlink_min();
		Deleter::operator()(old);
	}
	
	void erase(T* el)
	{
		unlink(el);
		Deleter::operator()(el);
	}
	
	void clear()
	{
		if(root)
		{
			delete_subtree_(root);
			root = 0;
			//n = 0;
		}
	}

	T& min()
	{
		passert(root, "Empty heap");
		return *downcast(root);
	}
		
private:
	
	void unlink_subtree_(pairing_node_common* el)
	{
		pairing_node_common** prev_next = el->prev_next;
		pairing_node_common* right_sibling = el->right_sibling;
		if(right_sibling)
			right_sibling->prev_next = prev_next;
		*prev_next = right_sibling;
	}
	
	// comparison_link_ ignores the value of right_sibling and prev_next
	// for both a and b.
	// BUT NOTE: Returned node has unmodified right_sibling and prev_next!
	pairing_node_common* comparison_link_(pairing_node_common* a, pairing_node_common* b)
	{
		if(Compare::operator()(*downcast(a), *downcast(b)))
		{
			// Make 'b' a child of 'a'
			b->prev_next = &a->left_child;
			
			pairing_node_common* ch = a->left_child;
			b->right_sibling = ch;
			if(ch)
				ch->prev_next = &b->right_sibling;
			a->left_child = b;
			return a;
		}
		else
		{
			// Make 'a' a child of 'b'
			a->prev_next = &b->left_child;
			
			pairing_node_common* ch = b->left_child;
			a->right_sibling = ch;
			if(ch)
				ch->prev_next = &a->right_sibling;
			b->left_child = a;
			return b;
		}
	}
	
	// NOTE: Return node has undefined right_sibling and prev!
	pairing_node_common* combine_siblings_(pairing_node_common* el)
	{
		sassert(el);
		
		//VL_PROF_COUNT("entry");
		
		pairing_node_common* first = el;
		pairing_node_common* second = first->right_sibling;
		
		if(!second)
			return first; // Only one sub-tree
		
		// We're fast-tracking the case with two children

		pairing_node_common* next = second->right_sibling;
			
		pairing_node_common* stack = comparison_link_(first, second);
		
		if(!next)
			return stack;
			
		stack->right_sibling = 0; // stack termination
		
		//VL_PROF_COUNT("more than two children");
		
		// First pass
		
		do
		{
			first = next;
			second = next->right_sibling;
			if(!second)
			{
				// Add first to the stack
				first->right_sibling = stack;
				stack = first;
				break;
			}
			
			next = second->right_sibling;
			
			pairing_node_common* tree = comparison_link_(first, second);
			// Add tree to the stack
			tree->right_sibling = stack;
			stack = tree;
		}
		while(next);
		
		// Second pass
		
		first = stack;
		second = stack->right_sibling;
		
		// The two-children test above guarantees there's at least two trees in the stack
		sassert(second);
		do
		{
			pairing_node_common* next = second->right_sibling;
			first = comparison_link_(first, second);
			second = next;
		}
		while(second);
		
		return first;
	}
	
	void delete_subtree_(pairing_node_common* el)
	{
		pairing_node_common* child = el->left_child;
		
		while(child)
		{
			pairing_node_common* next = child->right_sibling;
			delete_subtree_(child);
			child = next;
		}
		
		Deleter::operator()(el);
	}
	
	pairing_node_common* root; // root->prev_next and root->right_sibling are undefined
	//std::size_t n;
};

}

#endif // UUID_332507506BA94073DD795F88B83DFD8B

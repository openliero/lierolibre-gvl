#include <gvl/tut/tut.hpp>

#include <gvl/containers/pairing_heap.hpp>
#include <gvl/math/tt800.hpp>
#include <gvl/support/algorithm.hpp>
#include <functional>
#include <cmath>
#include <set>
#include <ctime>
#include <algorithm>
#include <set>

#include <gvl/tut/quickcheck/context.hpp>
#include <gvl/tut/quickcheck/generator.hpp>
#include <gvl/tut/quickcheck/property.hpp>

namespace tut
{

struct pairing_heap_integer : gvl::pairing_node<>
{
	pairing_heap_integer(int v)
	: v(v)
	{
	}
	
	bool operator<(pairing_heap_integer const& b) const
	{
		return v < b.v;
	}
	
	int v;
};

typedef gvl::pairing_heap<pairing_heap_integer> h_t;

#define TANDEM(x, op) do { (x)->first.op; (x)->second.op; } while(0)

template<typename T>
struct heap_model
{
	struct ptr_comp
	{
		bool operator()(T* a, T* b) const
		{
			return *a < *b;
		}
	};
	
	void insert(T* x)
	{ elements.insert(x); }
	
	void meld(heap_model& b)
	{
		elements.insert(b.elements.begin(), b.elements.end());
		b.elements.clear();
	}
	
	void erase_min()
	{
		elements.erase(elements.begin());
	}
	
	T& min()
	{
		return **elements.begin();
	}
	
	std::size_t size() const { return elements.size(); }
	bool empty() const { return elements.empty(); }
	
	std::multiset<T*, ptr_comp> elements;
};

typedef std::pair<h_t, heap_model<pairing_heap_integer> > test_type;

QC_BEGIN_GEN(empty_heap_gen, test_type)
	std::cout << "Empty";
	return new t;
QC_END_GEN()

QC_BEGIN_GEN(singleton_heap_gen, test_type)
	std::auto_ptr<t> ret(new t);
	int v = ctx.rand(10000);
	TANDEM(ret, insert(new pairing_heap_integer(v)));
	std::cout << "Singleton(" << v << ")";
	return ret.release();
QC_END_GEN()

QC_BEGIN_GEN(merge_heap_gen, test_type)
	if(ctx.generator_depth() > 10)
	{
		return ctx.generate<t>("singleton");
	}
	std::cout << "Meld(";
	std::auto_ptr<t> a(ctx.generate_any<t>());
	std::cout << ", ";
	std::auto_ptr<t> b(ctx.generate_any<t>());
	std::cout << ")";
	
	/*
	std::cout << "\n\n";
	a->first.print_tree();
	std::cout << "U\n";
	b->first.print_tree();
	std::cout << "=\n";
	*/
	a->first.meld(b->first);
	a->second.meld(b->second);
	/*
	a->first.print_tree();
	std::cout << "\n\n";
	*/
	return a.release();
QC_END_GEN()

QC_BEGIN_GEN(erase_min_heap_gen, test_type)
	std::cout << "EraseMin(";
	std::auto_ptr<t> a(ctx.generate_any<t>());
	std::cout << ")";
	if(!a->first.empty())
		a->first.erase_min();
	if(!a->second.empty())
		a->second.erase_min();
	return a.release();
QC_END_GEN()

QC_BEGIN_GEN(insert_heap_gen, test_type)
	std::cout << "Insert(";
	std::auto_ptr<t> a(ctx.generate_any<t>());
	
	int v = ctx.rand(10000);
	std::cout << ", " << v << ")";
	TANDEM(a, insert(new pairing_heap_integer(v)));
	return a.release();
QC_END_GEN()


QC_BEGIN_PROP(heap_integrity_property, test_type)
	bool check(gvl::qc::context& ctx, t& obj)
	{
		std::cout << std::endl << "=== heap_integrity_property ===" << std::endl;
		
		while(!obj.first.empty() && !obj.second.empty())
		{
			int a = obj.first.min().v;
			int b = obj.second.min().v;
			
			std::cout << "(" << a << " == " << b << ") ";
			
			if(a != b)
				return false;
			obj.first.erase_min();
			obj.second.erase_min();
		}
		
		std::cout << std::endl;
		
		return obj.first.empty() && obj.second.empty();
	}
QC_END_PROP()

struct pairing_heap_data
{
	
};

typedef test_group<pairing_heap_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::pairing_heap");
} // namespace

namespace tut
{

/*
template<typename T>
struct greater : std::binary_function<T const&, T const&, bool>
{
	bool operator()(T const& a, T const& b) const
	{
		return b < a;
	}
};*/

template<>
template<>
void object::test<1>()
{
	
	
	h_t heap;
	
	int const count = 1000;
	std::vector<int> added;
	gvl::tt800 rand(1);
	
	for(int i = 0; i < count; ++i)
	{
		int v = rand.range(0, 10000);
		
		added.push_back(v);
		heap.insert(new pairing_heap_integer(v));
	}
	
	std::sort(added.begin(), added.end());
	
	for(int i = 0; i < count; ++i)
	{
		ensure("not empty", !heap.empty());
		ensure("all items are present and popped in the right order", added[i] == heap.min().v);
		heap.erase_min();
	}
}

template<>
template<>
void object::test<2>()
{
	gvl::qc::context ctx;
	ctx.add("singleton", new singleton_heap_gen, 0.5);
	ctx.add("merge", new merge_heap_gen);
	ctx.add("insert", new insert_heap_gen, 1.5);
	ctx.add("empty", new empty_heap_gen, 0.2);
	ctx.add("erase_min", new erase_min_heap_gen, 0.4);
	
	gvl::qc::test_property<heap_integrity_property>(ctx);
}

} // namespace tut

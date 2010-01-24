#ifndef GVL_TUT_QUICKCHECK_CONTEXT_HPP
#define GVL_TUT_QUICKCHECK_CONTEXT_HPP

#include "../../support/type_info.hpp"
#include "../../resman/shared_ptr.hpp"
#include "../../math/cmwc.hpp"
#include "generator.hpp"

#include <map>
#include <string>
#include <stdexcept>


namespace gvl
{
namespace qc
{

struct generator_set
{
	generator_set()
	: total_weight(0.0)
	{
		
	}
	
	typedef std::map<std::string, base_generator*> generator_map;
	
	base_generator* operator[](std::string const& name)
	{
		return m[name];
	}
	
	template<typename T>
	void add(std::string const& name, generator<T>* g, double set_weight = 1.0)
	{
		g->weight = set_weight;
		if(!m.insert(std::make_pair(name, g)).second)
			throw std::runtime_error("A generator with this name is already present");
		total_weight += g->weight;
	}
	
	template<typename T>
	void add_reusable(shared_ptr_any<T> obj)
	{
		// TODO: Need static_cast for shared_ptr_any to
		// do this (maybe). We could make type-specific
		// generator_sets too.
	} 
	
	generator_map& all()
	{ return m; }
	
	generator_map m;
	
	double total_weight;
};

struct context
{
	static context* current;
	
	context()
	: generator_depth_(0)
	, assert_fails_(0)
	{
	}
	
	template<typename T>
	void add(std::string const& name, generator<T>* g, double set_weight = 1.0)
	{
		generator_set& m = generators[gvl::type_id<T>()];
		m.add(name, g, set_weight);
	}
	
	template<typename T>
	generator<T>& get_generator(std::string const& name)
	{
		generator_set& m = generators[gvl::type_id<T>()];
		generator_set::generator_map::iterator i = m.all().find(name);
		if(i == m.all().end())
			throw std::runtime_error("A generator with this name does not exist");
		generator<T>& p = *static_cast<generator<T>*>(i->second);
		return p;
	}
	
	template<typename T>
	shared_ptr_any<T> generate_any()
	{
		generator_set& m = generators[gvl::type_id<T>()];
		
		if(m.all().empty())
			throw std::runtime_error("There are no generators for this type");
			
		double n = rand.get_double(m.total_weight);
		generator_set::generator_map::iterator i = m.all().begin();
		for(; i != m.all().end(); ++i)
		{
			double weight = i->second->weight;
			if(n < weight)
				break;
			n -= weight;
		}
		if(i == m.all().end())
			throw std::runtime_error("Failed roulette-wheel selection");
		++generator_depth_;
		shared_ptr_any<T> p(static_cast<generator<T>*>(i->second)->gen_t(*this));
		--generator_depth_;
		return p;
	}
	
	
	
	template<typename T>
	shared_ptr_any<T> generate(std::string const& name)
	{
		++generator_depth_;
		shared_ptr_any<T> p(get_generator<T>(name).gen_t(*this));
		--generator_depth_;
		return p;
	}
	
	
	
	template<typename T>
	void add_reusable(shared_ptr_any<T> obj)
	{
		generator_set& m = generators[gvl::type_id<T>()];
		
		m.add_reusable(obj);
	}
	
	int generator_depth() const
	{ return generator_depth_; }
	
	bool assert_fail(char const* cond, char const* file, int line, char const* desc)
	{
		++assert_fails_;
		return false;
	}
	
	void reset_assert_fails()
	{ assert_fails_ = 0;}
	
	int get_assert_fails() const
	{ return assert_fails_; }
	
	std::map<gvl::type_info, generator_set> generators;
	gvl::mwc rand;
	int generator_depth_;
	int assert_fails_;
};



#define QC_ASSERT(desc, cond) if(!(cond)) \
{ if(!ctx.assert_fail(#cond, __FILE__, __LINE__, desc)) return ::gvl::qc::chk_fail; } else (void)0

#define QC_FATAL_ASSERT(desc, cond) if(!(cond)) \
{ ctx.assert_fail(#cond, __FILE__, __LINE__, desc); return ::gvl::qc::chk_fail; } else (void)0

} // namespace qc
} // namespace gvl

#endif // GVL_TUT_QUICKCHECK_CONTEXT_HPP

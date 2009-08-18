#ifndef GVL_TUT_QUICKCHECK_CONTEXT_HPP
#define GVL_TUT_QUICKCHECK_CONTEXT_HPP

#include "../../support/type_info.hpp"
#include "../../resman/shared_ptr.hpp"
#include "../../math/cmwc.hpp"
#include "generator.hpp"

#include <map>
#include <string>


namespace gvl
{
namespace qc
{

struct context
{
	typedef std::map<std::string, base_generator*> generator_map;
	
	template<typename T>
	void add(std::string const& name, generator<T>* g)
	{
		generator_map& m = generators[gvl::type_id<T>()];
		if(!m.insert(std::make_pair(name, g)).second)
			throw std::runtime_error("A generator with this name is already present");
	}
	
	template<typename T>
	generator<T>& get_generator(std::string const& name)
	{
		generator_map& m = generators[gvl::type_id<T>()];
		generator_map::iterator i = m.find(name);
		if(i == m.end())
			throw std::runtime_error("A generator with this name does not exist");
		return *static_cast<generator<T>*>(i->second);
	}
	
	template<typename T>
	T* generate_any()
	{
		generator_map& m = generators[gvl::type_id<T>()];
		
		if(m.empty())
			throw std::runtime_error("There are no generators for this type");
			
		uint32_t n = rand(m.size());
		generator_map::iterator i = m.begin();
		while(n-- > 0)
			++i;
		return static_cast<generator<T>*>(i->second)->gen_t(*this);
	}
	
	template<typename T>
	T* generate(std::string const& name)
	{
		return get_generator<T>(name).gen_t(*this);
	}
	
	std::map<gvl::type_info, generator_map> generators;
	gvl::mwc rand;
};

} // namespace qc
} // namespace gvl

#endif // GVL_TUT_QUICKCHECK_CONTEXT_HPP

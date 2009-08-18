#ifndef GVL_TUT_QUICKCHECK_PROPERTY_HPP
#define GVL_TUT_QUICKCHECK_PROPERTY_HPP

#include "generator.hpp"
#include "context.hpp"

#include <map>
#include <string>


namespace gvl
{
namespace qc
{

template<typename T>
struct property
{
	virtual T* generate(context& ctx)
	{
		return ctx.generate_any<T>();
	}
	
	virtual bool check(context&, T&) = 0;
	
	virtual bool holds_for(context&, T&)
	{
		return true; // All by default
	}
	
	
	bool run(context& ctx, int max_generated, int max_tested)
	{
		int tests_ran = 0;
        int generated = 0;

		for(generated = 0; generated < max_generated; )
        {
            shared_ptr_any<T> obj(generate(ctx));
            ++generated;
            if(holds_for(ctx, *obj))
            {
                ++tests_ran;
                if (!check(ctx, *obj))
                {
                    std::cout << "Property failed on test " << tests_ran << std::endl;
                    return false;
                }
                
                if (tests_ran >= max_tested)
                    break;
            }
        }
        
        std::cout << "OK, Ran " << tests_ran << " tests out of " << generated << " generated" << std::endl;
        
        return true;
	}
};

template<typename Prop>
bool test_property(context& ctx, int max_generated = 1000, int max_tested = 500)
{
	Prop prop;
	return prop.run(ctx, max_generated, max_tested);
}

} // namespace qc
} // namespace gvl

#endif // GVL_TUT_QUICKCHECK_PROPERTY_HPP

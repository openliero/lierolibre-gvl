#ifndef GVL_TUT_QUICKCHECK_GENERATOR_HPP
#define GVL_TUT_QUICKCHECK_GENERATOR_HPP

#include <map>
#include <string>

namespace gvl
{
namespace qc
{

struct context;

struct base_generator
{
	base_generator(double weight = 1.0)
	: weight(weight)
	{
	}
	
	virtual void* gen(context& ctx) = 0;
	
	double weight;
};

template<typename T>
struct generator : base_generator
{
	generator(double weight = 1.0)
	: base_generator(weight)
	{
	}
	
	void* gen(context& ctx) { return gen_t(ctx); }
	virtual T* gen_t(context& ctx) = 0;
};


#define QC_BEGIN_GEN(name, type) \
struct name : gvl::qc::generator<type> { \
	typedef type t; \
	t* gen_t(gvl::qc::context& ctx) {

#define QC_END_GEN() } };


} // namespace qc
} // namespace gvl

#endif // GVL_TUT_QUICKCHECK_GENERATOR_HPP

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
	virtual void* gen(context& ctx) = 0;
};

template<typename T>
struct generator : base_generator
{
	void* gen(context& ctx) { return gen_t(ctx); }
	virtual T* gen_t(context& ctx) = 0;
};

} // namespace qc
} // namespace gvl

#endif // GVL_TUT_QUICKCHECK_GENERATOR_HPP

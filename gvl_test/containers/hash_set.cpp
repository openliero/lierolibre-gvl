#include <tut/tut.hpp>

#include <set>
#include <gvl/containers/hash_set.hpp>
#include <tut/quickcheck/quickcheck.hpp>
#include <gvl/containers/range.hpp>
#include <gvl/support/foreach.hpp>

namespace tut
{

typedef gvl::hash_set<int> integer_hash_set;

#define TANDEM(x, op) do { (x)->first.op; (x)->second.op; } while(0)

template<typename T>
struct hash_model
{
	typedef typename std::set<T>::iterator iterator;

	void insert(T const& v)
	{ s.insert(v); }
	void erase(T const& v)
	{ s.erase(v); }

	iterator begin()
	{ return s.begin(); }

	iterator end()
	{ return s.end(); }

	std::set<T> s;
};

typedef std::pair<integer_hash_set, hash_model<int> > test_type;

QC_BEGIN_GEN(empty_hash_gen, test_type)
	return ptr_t(new t);
QC_END_GEN()

QC_BEGIN_GEN(singleton_hash_gen, test_type)
	ptr_t ret(new t);
	uint32_t v = ctx.rand(0xffffff);
	TANDEM(ret, insert(v));
	return ret;
QC_END_GEN()

QC_BEGIN_GEN(insert_hash_gen, test_type)
	ptr_t ret(ctx.generate_any<t>());
	uint32_t v = ctx.rand(0xffffff);
	TANDEM(ret, insert(v));
	return ret;
QC_END_GEN()

QC_BEGIN_GEN(erase_hash_gen, test_type)
	ptr_t ret(ctx.generate_any<t>());
	uint32_t v = ctx.rand(0xffffff);
	TANDEM(ret, erase(v));
	return ret;
QC_END_GEN()

QC_BEGIN_PROP(hash_integrity_property, test_type)
	chk_result check(gvl::qc::context& ctx, QC_GEN_ANY(t, obj))
	{
		std::vector<int> first_cont, second_cont;

		GVL_FOREACH(int i, gvl::iter_range(obj->first))
		{
			first_cont.push_back(i);
		}

		GVL_FOREACH(int i, gvl::iter_range(obj->second))
		{
			second_cont.push_back(i);
		}

		std::sort(first_cont.begin(), first_cont.end());
		std::sort(second_cont.begin(), second_cont.end());

		QC_ASSERT("same number of elements", first_cont.size() == second_cont.size());

		for(std::size_t i = 0; i < first_cont.size(); ++i)
		{
			QC_ASSERT("elements match", first_cont[i] == second_cont[i]);
		}

		return chk_ok_reuse;
	}
QC_END_PROP()

struct hash_set_data
{

};

typedef test_group<hash_set_data> factory;
typedef factory::object object;

}

namespace
{
	tut::factory tf("gvl::hash_set");
} // namespace

namespace tut
{


template<>
template<>
void object::test<1>()
{
	gvl::qc::context ctx;
	ctx.add("singleton", new singleton_hash_gen, 2.0);
	ctx.add("empty", new empty_hash_gen);
	ctx.add("insert", new insert_hash_gen, 100.0);
	ctx.add("erase", new erase_hash_gen, 100.0);

	gvl::qc::test_property<hash_integrity_property>(ctx);
}


} // namespace tut

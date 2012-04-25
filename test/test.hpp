/*
 * Copyright (c) 2010, Erik Lindroos <gliptic@gmail.com>
 * This software is released under the The BSD-2-Clause License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UUID_A034FAE8950C4164C75E3B8BEDA507CC
#define UUID_A034FAE8950C4164C75E3B8BEDA507CC

#include "../list.hpp"
#include "../io/iostream.hpp"
#include "../io/encoding_compat.hpp"
#include <map>
#include <string>

namespace gvl
{

struct test_case_instance;
struct test_context;

//test_context& test_context_singleton();

struct test_case : gvl::list_node<>
{
	test_case(std::string const& suite_name, test_context& (*get_context_init)())
	: get_context(get_context_init)
	{
		register_(suite_name);
	}

	inline void register_(std::string const& suite_name);

	virtual void run(test_case_instance& _test_case_instance) = 0;

	test_context& (*get_context)();
};

struct test_case_instance
{
	enum
	{
		failed = (1<<0),
		warning = (1<<1)
	};

	test_case_instance(test_case* tc_init)
	: flags(0)
	, tc(tc_init)
	, failures_until_mute(30)
	{
	}

	bool should_report() const
	{ return failures_until_mute > 0; }

	void mark_failed()
	{
		flags |= failed;
		--failures_until_mute;
	}

	void mark_warning()
	{
		flags |= warning;
	}

	int flags;
	test_case* tc;
	int failures_until_mute;
};

struct test_suite_instance
{
	test_suite_instance()
	: flags(0)
	, failures(0)
	, successes(0)
	{
	}

	int total() const
	{ return failures + successes; }

	int flags;
	int failures;
	int successes;
};

struct test_run_instance
{
	test_run_instance()
	: failures(0)
	{
	}

	int failures;
};

struct test_suite
{
	test_suite(std::string const& name)
	: name(name)
	{
	}

	void register_(test_case* test)
	{
		tests.push_back(test);
	}

	void run_all(test_suite_instance& suite_instance);

	std::size_t size() const
	{ return tests.size(); }

	std::string name;
	gvl::weak_list<test_case> tests;
};

struct test_context
{
	typedef std::map<std::string, test_suite*> test_suite_map_t;
	test_suite_map_t suites;

	test_context()
	: dout(cout())
	{
	}

	std::size_t test_case_count() const
	{
		std::size_t count = 0;
		for(auto it = suites.begin(); it != suites.end(); ++it)
		{
			count += it->second->size();
		}
		return count;
	}

	inline test_suite& test_context::get_test_suite(std::string const& name);
	inline void run_all_suites();

	octet_stream_writer& dout;
};

inline void test_suite::run_all(test_suite_instance& suite_instance)
{
	weak_list<test_case>::iterator i = tests.begin();

	for(; i != tests.end(); ++i)
	{
		gvl::test_case_instance instance(&*i);
		i->run(instance);

		suite_instance.flags |= instance.flags;
		if(instance.flags & test_case_instance::failed)
			++suite_instance.failures;
		else
			++suite_instance.successes;
	}


}

inline test_suite& test_context::get_test_suite(std::string const& name)
{
	test_suite*& s = suites[name];
	if(!s) s = new test_suite(name);
	return *s;
}

inline void test_context::run_all_suites()
{
	test_context::test_suite_map_t::iterator i = this->suites.begin();

	octet_stream_writer& dout = this->dout;

	test_run_instance run_instance;

	dout << "@(TESTS " << this->test_case_count() << ")\n";
	dout.flush();

	for(; i != this->suites.end(); ++i)
	{
		test_suite_instance instance;
		i->second->run_all(instance);

		dout << "@(SUITE " << i->second->name << "): ";
		dout << instance.successes << "/" << instance.total() << "\n";
		dout.flush();

		if(instance.failures > 0)
			++run_instance.failures;
	}
}

inline void test_case::register_(std::string const& suite_name)
{
	get_context().get_test_suite(suite_name).register_(this);
}

#define GVLTEST_DEF_CONTEXT(context) \
namespace gvl { ::gvl::test_context& test_context_##context() { \
	static test_context x; \
	return x; \
} }

#define GVLTEST_DECL_CONTEXT(context) \
namespace gvl { ::gvl::test_context& test_context_##context(); }

#define GVLTEST_CONTEXT(context) (::gvl::test_context_##context())

#define GVLTEST_SUITE(context, suite) \
	GVLTEST_DECL_CONTEXT(context) \
	namespace gvl { \
	int _test_suite_reg##context##suite() { return 0; } }

#define GVLTEST_REG_SUITE(context, suite) \
GVLTEST_DECL_CONTEXT(context) \
namespace gvl { int _test_suite_reg##context##suite(); } \
static int _dummy_##context##suite = ::gvl::_test_suite_reg##context##suite();

#define GVLTEST(context, suite, name) \
struct test_##context##suite##_##name : ::gvl::test_case { \
test_##context##suite##_##name( \
	std::string const& suite_name, \
	::gvl::test_context& (*get_context)()) : ::gvl::test_case(suite_name, get_context) \
{ } \
void run(::gvl::test_case_instance&); \
} instance_##context##suite##_##name(#suite, \
	::gvl::test_context_##context); \
void test_##context##suite##_##name::run(::gvl::test_case_instance& _test_case_instance)

template<typename TA, typename TB>
void test_assert_equal(
	test_case_instance& self, char const* msg, TA const& a, TB const& b,
	char const* astr, char const* bstr, char const* file, int line)
{
	if(a != b)
	{
		if(self.should_report())
		{
			gvl::octet_stream_writer& dout = self.tc->get_context().dout;
			dout << file << '(' << line << "): (" << astr << ") != (" << bstr << "), " << msg << '\n';
			dout << "\t" << astr << " == " << a << "\n";
			dout << "\t" << bstr << " == " << b << "\n";
		}

		self.mark_failed();
	}
}

inline void test_assert_failed(
	test_case_instance& self, char const* msg,
	char const* condstr, char const* file, int line)
{
	if(self.should_report())
	{
		gvl::octet_stream_writer& dout = self.tc->get_context().dout;
		dout << file << '(' << line << "): !(" << condstr << "), " << msg << '\n';
	}

	self.mark_failed();
}

#define GVLTEST_EQUAL(a, b) ::gvl::test_assert_equal(_test_case_instance, "assert failed", (a), (b), #a, #b, __FILE__, __LINE__)
#define GVLTEST_EQUALM(m, a, b) ::gvl::test_assert_equal(_test_case_instance, (m), (a), (b), #a, #b, __FILE__, __LINE__)

#define GVLTEST_ASSERT(m, cond) if(!(cond)) ::gvl::test_assert_failed(_test_case_instance, (m), #cond, __FILE__, __LINE__); else (void)0

#if !GVLTEST_NOABBREV
#define ASSERT GVLTEST_ASSERT
#define ASSERTEQ GVLTEST_EQUAL
#define ASSERTEQM GVLTEST_EQUALM
#endif

}

#endif // UUID_A034FAE8950C4164C75E3B8BEDA507CC

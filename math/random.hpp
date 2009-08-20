#ifndef UUID_F29926F3240844A09DCFB9B1828C7DC8
#define UUID_F29926F3240844A09DCFB9B1828C7DC8

namespace gvl
{

template<typename DerivedT, typename ValueT>
struct prng_common
{
	typedef ValueT value_type;
	
	DerivedT& derived()
	{ return *static_cast<DerivedT*>(this); }
	
	double get_double(double max)
	{
		uint32_t v = derived()();
		return v * max / 4294967296.0;
	}
	
	uint32_t operator()(uint32_t max)
	{
		uint64_t v = derived()();
		v *= max;
		return uint32_t(v >> 32);
	}
	
	uint32_t operator()(uint32_t min, uint32_t max)
	{
		return derived()(max - min) + min;
	}
};

} // namespace gvl

#endif // UUID_F29926F3240844A09DCFB9B1828C7DC8


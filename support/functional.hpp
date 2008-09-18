#ifndef UUID_64B1EAC4E4F545FA3B131FA346621126
#define UUID_64B1EAC4E4F545FA3B131FA346621126

namespace gvl
{

struct default_delete
{
	template<typename T>
	void operator()(T* p) const
	{
		delete p;
	}
};

struct dummy_delete
{
	template<typename T>
	void operator()(T const&) const
	{
		// Do nothing
	}
};

} // namespace gvl

#endif // UUID_64B1EAC4E4F545FA3B131FA346621126

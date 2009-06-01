#ifndef UUID_C4C5ABCE77EF40A7F785AFB143D018B0
#define UUID_C4C5ABCE77EF40A7F785AFB143D018B0

namespace gvl
{

template<typename T>
inline T clamp(T value, T min, T max)
{
	if(value < min)
		return min;
	else if(value > max)
		return max;
	else
		return value;
}

}

#endif // UUID_C4C5ABCE77EF40A7F785AFB143D018B0

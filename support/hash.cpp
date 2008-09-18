#include "hash.hpp"
#include "debug.hpp"

namespace gvl
{


std::size_t primes[] =
{
	//5,          //2^2  + 1
	11,         //2^3  + 3
	17,         //2^4  + 1
	37,         //2^5  + 5
	67,         //2^6  + 3
	131,        //2^7  + 3
	257,        //2^8  + 1
	521,        //2^9  + 9
	1031,       //2^10 + 7
	2053,       //2^11 + 5
	4099,       //2^12 + 3
	8209,       //2^13 + 17
	16411,      //2^14 + 27
	32771,      //2^15 + 3
	65537,      //2^16 + 1
	131101,     //2^17 + 29
	262147,     //2^18 + 3
	524309,     //2^19 + 21
	1048583,    //2^20 + 7
	2097169,    //2^21 + 17
	4194319,    //2^22 + 15
	8388617,    //2^23 + 9
	16777259,   //2^24 + 43
	33554467,   //2^25 + 35
	67108879,   //2^26 + 15
	134217757,  //2^27 + 29
	268435459,  //2^28 + 3
	536870923,  //2^29 + 11
	1073741827, //2^30 + 3
	0
};

std::size_t next_prime(std::size_t n)
{
	std::size_t i = 0;
	for(; n > primes[i]; ++i)
	{
		sassert(primes[i]);
	}
	
	return primes[i];
}

std::size_t prev_prime(std::size_t n)
{
	std::size_t i = 1;
	for(; n > primes[i] && primes[i]; ++i)
	{
		sassert(primes[i]);
	}
	
	return primes[i-1];
}

} // namespace gvl

#ifndef UUID_EA9A2A74DD9448CF4ABAC285FDC42F3A
#define UUID_EA9A2A74DD9448CF4ABAC285FDC42F3A

#include "../support/debug.hpp"
#include "../support/opt.hpp"

namespace gvl
{

prepared_division const& get_base_divider(int base)
{
	static prepared_division base_dividers[36-1] =
	{
		prepared_division(2), prepared_division(3),
		prepared_division(4), prepared_division(5),
		prepared_division(6), prepared_division(7),
		prepared_division(8), prepared_division(9),
		prepared_division(10), prepared_division(11),
		prepared_division(12), prepared_division(13),
		prepared_division(14), prepared_division(15),
		prepared_division(16), prepared_division(17),
		prepared_division(18), prepared_division(19),
		prepared_division(20), prepared_division(21),
		prepared_division(22), prepared_division(23),
		prepared_division(24), prepared_division(25),
		prepared_division(26), prepared_division(27),
		prepared_division(28), prepared_division(29),
		prepared_division(30), prepared_division(31),
		prepared_division(32), prepared_division(33),
		prepared_division(34), prepared_division(35),
		prepared_division(36)
	};
	
	sassert(base >= 2 && base <= 36);
	
	return base_dividers[base-2];
}

template<typename Writer, typename T>
int uint_to_ascii(Writer& writer, T x, int base = 10, int min_digits = 1, uint8_t over_9 = 'a')
{
	if(base < 2 || base > 36)
		return -1;
		
	prepared_division div = get_base_divider(base);
	
	std::size_t const buf_size = sizeof(T) * CHAR_BIT;
	uint8_t digits[buf_size];
	uint8_t* e = digits + buf_size;
	uint8_t* p = e;

	while(min_digits-- > 0 || x > 0)
	{
		std::pair<uint32_t, uint32_t> res(div.quot_rem(x));
		if(res.second >= 10)
			res.second += over_9 - '0' - 10;
		*--p = res.second + '0';
		
		x = res.first;
	}
  
	writer.put(p, e - p);
	return 0;
}

template<uint32_t Base, typename Writer, typename T>
int uint_to_ascii_base(Writer& writer, T x, int base = 10, int min_digits = 1, uint8_t over_9 = 'a')
{
	std::size_t const buf_size = sizeof(T) * CHAR_BIT;
	uint8_t digits[buf_size];
	uint8_t* e = digits + buf_size;
	uint8_t* p = e;

	while(min_digits-- > 0 || x > 0)
	{
		int n = x % Base;
		if(Base > 10 && n >= 10)
			n += over_9 - '0' - 10;
		*--p = n + '0';		
		x /= Base;
	}
  
	writer.put(p, e - p);
	return 0;
}

template<typename Writer, typename T>
void int_to_ascii(Writer& writer, T x, int base = 10, int min_digits = 1, uint8_t over_9 = 'a')
{
	if(x < 0)
	{
		writer.put('-');
		uint_to_ascii<Base>(writer, -x, base, min_digits, over_9);
	}
	else
	{
		uint_to_ascii<Base>(writer, x, base, min_digits, over_9);
	}
}

}

#endif // UUID_EA9A2A74DD9448CF4ABAC285FDC42F3A

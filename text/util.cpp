#include "util.hpp"

#include <cctype>

namespace gvl
{

void trim(std::string& str)
{
	std::size_t b = 0;

	while(b < str.size() && std::isspace((unsigned char)str[b]))
		++b;

	std::size_t e = str.size();

	while(b < e && std::isspace((unsigned char)str[e - 1]))
		--e;

	str = str.substr(b, e - b);
}

}

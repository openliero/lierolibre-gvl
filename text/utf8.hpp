#ifndef UUID_A91D620921B84FAFD328BABED6AA9CF2
#define UUID_A91D620921B84FAFD328BABED6AA9CF2

#include "../support/cstdint.hpp"
#include "../support/debug.hpp"

namespace gvl
{

namespace utf8
{

uint32_t const invalid_codepoint = uint32_t(-1);

// Length of a character starting with byte 'start'.
// NOTE: This assumes the byte is a valid first byte of
// a character in UTF8. Return value is undefined in
// all other cases.
inline int unsafe_char_length(uint8_t start)
{
	uint32_t high_nibble = (start >> 4);
	
	// Nibble 0xC is 2 bytes, 0xD is 3 bytes, 0xE is 4 bytes
	return std::max(high_nibble - 0xC + 2, 1);
}

inline bool is_middle_byte(uint8_t b)
{
	return (b & 0xc0) == 0x80;
}

template<typename Range>
inline uint32_t unsafe_decode(Range& range)
{
	uint32_t A = range.front();
	range.pop_front();
	
	if(A < 0x80)
	{
		return A; // ASCII
	}
#ifndef NDEBUG
	else if(A < 0xc2)
	{
		// Middle bytes or overlong 2-byte sequences
		passert(false, "Invalid sequence");
	}
#endif
	else if(A < 0xe0)
	{
		// 2-byte sequence
		uint32_t B = range.front(); range.pop_front();
		passert(is_middle_byte(B), "Expected middle byte");
		return ((A & 0x1f) << 6) | (B & 0x3f);
	}
	else if(A < 0xf0)
	{
		// 3-byte sequence
		uint32_t B = range.front(); range.pop_front();
		uint32_t C = range.front(); range.pop_front();
		passert(is_middle_byte(B), "Expected middle byte");
		passert(is_middle_byte(C), "Expected middle byte");
		return ((A & 0xf) << 12) | ((B & 0x3f) << 6) | (C & 0x3f);
	}
	else if(A < 0xf5)
	{
		// 4-byte sequence
		uint32_t B = range.front(); range.pop_front();
		uint32_t C = range.front(); range.pop_front();
		uint32_t D = range.front(); range.pop_front();
		passert(is_middle_byte(B), "Expected middle byte");
		passert(is_middle_byte(C), "Expected middle byte");
		passert(is_middle_byte(D), "Expected middle byte");
		return ((A & 0x7) << 18) | ((B & 0x3f) << 12) | ((C & 0x3f) << 6) | (C & 0x3f);
	}
	else
		passert(false, "Invalid sequence");
		
	return '?';
}

inline uint32_t is_illegal_codepoint(uint32_t v)
{
	if((v > 0xD7FF && v < 0xE000)
	|| v > 0x10FFFF)
		return true;
	return false;
}

template<typename Range>
uint32_t decode(Range& range, uint32_t illegal_replacement = invalid_codepoint)
{
	uint32_t A = range.front();
	range.pop_front();
	
	Range invalid_range = range; // Invalid characters consume one byte
	
	if(A < 0x80)
	{
		return A;
	}
	else if(A < 0xc2)
	{
		goto invalid;
	}
	else if(A < 0xe0)
	{
		// 2-byte sequence
		if(range.empty())
			goto invalid;
		uint32_t B = range.front(); range.pop_front();
		if(!is_middle_byte(B))
			goto invalid;
		uint32_t v = ((A & 0x1f) << 6) | (B & 0x3f);
		if(v < 0x80 || is_illegal_codepoint(v))
			goto invalid; // Overlong form or illegal
		return v;
	}
	else if(A < 0xf0)
	{
		// 3-byte sequence
		if(range.empty())
			goto invalid;
		uint32_t B = range.front(); range.pop_front();
		if(!is_middle_byte(B) || range.empty())
			goto invalid;
		uint32_t C = range.front(); range.pop_front();
		if(!is_middle_byte(C))
			goto invalid;
		uint32_t v = ((A & 0xf) << 12) | ((B & 0x3f) << 6) | (C & 0x3f);
		if(v < 0x800 || is_illegal_codepoint(v))
			goto invalid; // Overlong form or illegal
		return v;
	}
	else if(A < 0xf5)
	{
		// 4-byte sequence
		if(range.empty())
			goto invalid;
		uint32_t B = range.front(); range.pop_front();
		if(!is_middle_byte(B) || range.empty())
			goto invalid;
		uint32_t C = range.front(); range.pop_front();
		if(!is_middle_byte(C) || range.empty())
			goto invalid;
		uint32_t D = range.front(); range.pop_front();
		if(!is_middle_byte(D))
			goto invalid;
		uint32_t v = ((A & 0x7) << 18) | ((B & 0x3f) << 12) | ((C & 0x3f) << 6) | (C & 0x3f);
		if(v < 0x10000 || is_illegal_codepoint(v))
			goto invalid; // Overlong form or illegal
		return v;
	}
	
invalid:
	range = invalid_range;
	return invalid_codepoint;
}

void normalize(std::string& str)
{
	
}

} // namespace utf8

} // namespace gvl

#endif // UUID_A91D620921B84FAFD328BABED6AA9CF2

#include "critbit.hpp"

#include "../support/cstdint.hpp"
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace gvl
{

struct critbit_node
{
	void* children[2];
	uint32_t byte;
	uint8_t otherbits;
	
	static critbit_node* alloc()
	{
		return reinterpret_cast<critbit_node*>(std::malloc(sizeof(critbit_node)));
	}
};

struct critbit_external_node
{
	std::size_t len;
	uint8_t data[1];
	
	static std::size_t size(std::size_t len)
	{
		return sizeof(critbit_external_node) + len - sizeof(uint8_t);
	}
	
	static critbit_external_node* alloc(std::size_t len)
	{
		critbit_external_node* self = reinterpret_cast<critbit_external_node*>(std::malloc(size(len)));
		
		self->len = len;
		return self;
	}
};

uint8_t* walk(uint8_t* p, uint8_t const* u, std::size_t ulen)
{
	while(1 & (intptr_t)p)
	{
		critbit_node* q = reinterpret_cast<critbit_node*>(p - 1);
		
		uint8_t c = 0;
		uint32_t byte = q->byte;
		if(byte < ulen)
			c = u[byte];
		int const direction = (1 + (q->otherbits | c)) >> 8;
		
		p = static_cast<uint8_t*>(q->children[direction]);
	}
	
	return p;
}

bool critbit::contains(uint8_t const* u, std::size_t ulen)
{
	if(!root)
		return false;
	
	uint8_t* p = static_cast<uint8_t*>(root);
	
	p = walk(p, u, ulen);
	
	critbit_external_node* en = reinterpret_cast<critbit_external_node*>(p);
	
	if(en->len != ulen)
		return false;
	
	return 0 == std::memcmp(u, en->data, ulen);
}

int critbit::insert(uint8_t const* u, std::size_t ulen)
{
	uint8_t* p = static_cast<uint8_t*>(root);
	
	if(!p)
	{
		critbit_external_node* x = critbit_external_node::alloc(ulen);
		std::memcpy(x->data, u, ulen);
		root = x;
		return 2;
	}
	
	p = walk(p, u, ulen);
	
	critbit_external_node* en = reinterpret_cast<critbit_external_node*>(p);
	
	uint32_t newbyte, newotherbits;
	std::size_t min_len = std::min(ulen, en->len);
	
	uint8_t c;
		
	for(newbyte = 0; newbyte < min_len; ++newbyte)
	{
		uint8_t pbyte = en->data[newbyte];
		uint8_t ubyte = u[newbyte];
		if(pbyte != ubyte)
		{
			newotherbits = pbyte ^ ubyte;
			c = en->data[newbyte];
			goto different_byte_found;
		}
	}
	
	if(ulen > en->len)
	{
		newotherbits = u[en->len];
		c = 0;
	}
	else if(ulen < en->len)
	{
		newotherbits = en->data[ulen];
		c = en->data[ulen];
	}
	else
		return 1;
	
different_byte_found:
	// Leave last bit set
	while(newotherbits & (newotherbits - 1))
		newotherbits &= newotherbits - 1;
		
	newotherbits ^= 0xff;
	
	int newdirection = (1 + (newotherbits | c)) >> 8;
	
	critbit_node* newnode = critbit_node::alloc();
	critbit_external_node* x = critbit_external_node::alloc(ulen);
	std::memcpy(x->data, u, ulen);
	newnode->byte = newbyte;
	newnode->otherbits = newotherbits;
	newnode->children[1 - newdirection] = x;
	
	void** wherep = &root;
	for(;;)
	{
		p = static_cast<uint8_t*>(*wherep);
		
		if(!(1 & (intptr_t)p))
			break;
			
		critbit_node* q = reinterpret_cast<critbit_node*>(p - 1);
		
		if(q->byte > newbyte)
			break;
		if((q->byte == newbyte) ^ (q->otherbits > newotherbits))
			break;
			
		c = 0;
		if(q->byte < ulen)
			c = en->data[q->byte];
		int direction = (1 + (q->otherbits | c)) >> 8;
		wherep = q->children + direction;
	}
	
	newnode->children[newdirection] = *wherep;
	*wherep = 1 + reinterpret_cast<uint8_t*>(newnode);
	
	return 2;
}

}

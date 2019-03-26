#include "Allocator.hpp"
#include <cassert>
#include <cstring>

#ifndef ALIGN
#define ALIGN(x, a)				(((x) + ((a) - 1)) & ~((a) - 1))
#endif

using namespace Panda;

Panda::Allocator::Allocator(size_t data_size, size_t page_size, size_t alignment)
	: m_pPageList(nullptr), m_pFreeList(nullptr)
{
	Reset(data_size, page_size, alignment);
}

Panda::Allocator::~Allocator() 
{
	FreeAll();
}

void Panda::Allocator::Reset(size_t data_size, size_t page_size, size_t alignment)
{
	FreeAll();
	
	m_szDataSize = data_size;
	m_szPageSize = page_size；
	
	size_t minimal_size = (sizeof(BlockHeader) > m_szDataSize) ? sizeof (BlockHeader) : m_szDataSize;
	
	// this magic only works when alignment is 2^n, which should general be the case
	// because most CPU/GPU also requres the alignment be in 2^name
	// but still we use a assert to guarantee it
#if defined(_DEBUG)
	assert(alighment > 0 && ((alighment & (alignment - 1))) == 0);
#endif
	m_szBlockSize = ALIGN(minimal_size, alignment);
}
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
	m_szBlockSize = ALIGN(minimal_size, alignment);	// block size must match the alignment
	
	m_szAlignmentSize = m_szBlockSize - minimal_size;
	
#if defined(_DEBUG)
	assert(m_szPageSize - sizeof(PageHeader) - m_szBlockSize > 0);	// at least one block per page
#endif
	
	m_nBlocksPerPage = (m_szPageSize - sizeof (PageHeader)) / m_szBlockSize;
}

// Allocate a new block
void* Panda::Allocator::Allocate() {
	if (!m_pFreeList) {
		// allocate a new pate
		PageHeader* pNewPage = reinterpret_cast<PageHeader*>(new uint8_t[m_szPageSize]);
		++m_nPages;
		m_nBlocks += m_nBlockPerPage;	// 总块数
		m_nFreeBlocks += m_nBlockPerPage;	// 未使用块数
		
#if defined (_DEBUG)
		FillFreePage(pNewPage);
#endif

		if (m_pPageList) {
			pNewPage->pNext = m_pPageList;
		}
		
		m_pPageList = pNewPage;
		
		BlockHeader* pBlock = pNewPage->Blocks();
		// link each block in the page
		for (uint32_t i = 0; i < m_nBlockPerPage; ++i) {
			pBlock->pNext = NextBlock(pBlock);
			pBlock = pBlock->pNext;
		}
		pBlock->pNext = nullptr;
		
		m_pFreeList = pNewPage->Blocks();
	}
	
	BlockHeader* freeBlock = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	--m_nFreeBlocks;
	
#if defined (_DEBUG)
	FillAllocatedBlock(freeBlock);
#endif
	return reinterpret_cast<void*>(freeBlock);
}

void Panda::Allocator::Free(void* p) {
	BlockHeader* block = reinterpret_cast<BlockHeader*>(p);
	
#if defined(_DEBUG)
	FillFreeBlock(block);
#endif

	block->pNext = m_pFreeList;
	m_pFreeList = block;
	++m_nFreeBlocks;
}

void Panda::Allocator::FreeAll() {
	PageHeader* pPage = m_pPageList;
	while (pPage) {
		PageHeader* _p = pPage;
		pPage = pPage->pNext;
		
		delete[] reinterpret_cast<uint8_t*>(_p);
	}
	
	m_pPageList = nullptr;
	m_pFreeList = nullptr;
	
	m_nPages	= 0;
	m_nBlocks	= 0;
	m_nFreeBlocks = 0;
}

#if defined (_DEBUG)
void Panda::Allocator::FillFreePage(PageHeader* pPage) {
	// page header
	pPage->pNext = nullptr;
	
	// blocks
	BlockHeader* pBlock = pPage->Blocks();
	for (uint32_t i = 0; i < m_nBlockPerPage; ++i) {
		FillFreeBlock(pBlock);
		pBlock = NextBlock(pBlock);
	}
}

void Panda::Allocator::FillFreeBlock(BlockHeader* pBlock) {
	// block header + data
	std::memset(pBlock, PATTERN_FREE, m_szBlockSize - m_szAlignmentSize);
	
	// alignment
	std::memset(reinterpret_cast<uint8_t*>(pBlock) + m_szBlockSize - m_szAlignmentSize,
				PATTERN_ALIGN, m_szAlignmentSize);
}

void Panda::Allocator::FillAllocatedBlock(BlockHeader* pBlock) {
	// block header + data
	std::memset(pBlock, PATTERN_ALLOC, m_szBlockSize - m_szAlignmentSize);
	
	// alignment
	std::memset(reinterpret_cast<uint8_t*>(pBlock) + m_szBockSize - m_szAlignmentSize,
				PATTERN_ALIGN, m_szAlignmentSize);
}
#endif

Panda::BlockHeader* Panda::Allocator::NextBlock(BlockHeader* pBlock) {
	return reinterpret_cast<BlockHeader*>(reinterpret_cast<uint8_t*>(pBlock) + m_szBlockSize);
}
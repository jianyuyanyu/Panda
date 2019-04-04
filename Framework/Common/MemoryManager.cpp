#include "MemoryManager.hpp"
#include <malloc.h>

using namespace Panda;

bool MemoryManager::Initialize() {
	static bool s_bInitialized = false;
	
	if (!s_bInitialized) 
	{
		// 初始化分配器
		m_pAllocators = new Allocator[k_BlockSizeCount];
		for (size_t i = 0; i < k_BlockSizeCount; ++i) 
		{
			m_pAllocators[i].Reset(k_BlockSizes[i], k_PageSize, k_Alignment);
		}

		// 初始化查询表
		m_pLookUpTable = new uint32_t[k_MaxBlockSize + 1];
		size_t j = 0;
		for (size_t i = 0; i <= k_MaxBlockSize; ++i) 
		{
			if (i > k_BlockSizes[j]) 
			{
				++j;
			}
			
			m_pLookUpTable[i] = j;
		}
	
		s_bInitialized = true;
	}

	return true;
}

void MemoryManager::Finalize()
{
	delete[] m_pAllocators;
	delete[] m_pLookUpTable;
}

void MemoryManager::Tick()
{
}

void* MemoryManager::Allocate(size_t inSize)
{
	Allocator* pAlloc = LookUpAllocator(inSize);
	
	if (pAlloc)
		return pAlloc->Allocate();
	else
		return malloc(inSize);
}

void MemoryManager::Free(void* p, size_t inSize) 
{
	Allocator* pAlloc = LookUpAllocator(inSize);
	
	if (pAlloc)
		pAlloc->Free(p);
	else
		free(p);
}

Allocator* MemoryManager::LookUpAllocator(size_t inSize)
{
	if (inSize <= k_MaxBlockSize)
		return m_pAllocator + m_pLookUpTable[inSize];
	else
		return nullptr;
}

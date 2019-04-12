#include "MemoryManager.hpp"
#include <malloc.h>

using namespace Panda;

namespace Panda
{
	Allocator* MemoryManager::m_pAllocators = nullptr;
	uint32_t* MemoryManager::m_pLookUpTable = nullptr;
}

int Panda::MemoryManager::Initialize() {
	static bool s_bInitialized = false;
	
	if (!s_bInitialized) 
	{
		// 初始化分配器
		m_pAllocators = new Allocator[k_BlockSizeCount];
		for (size_t i = 0; i < k_BlockSizeCount; ++i) 
		{
			m_pAllocators[i].Reset(k_PageSize, k_BlockSizes[i], k_Alignment);
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

	return 0;
}

void Panda::MemoryManager::Finalize()
{
	delete[] m_pAllocators;
	delete[] m_pLookUpTable;
}

void Panda::MemoryManager::Tick()
{
}

void* Panda::MemoryManager::Allocate(size_t inSize)
{
	Allocator* pAlloc = LookUpAllocator(inSize);
	
	if (pAlloc)
		return pAlloc->Allocate();
	else
		return malloc(inSize);
}

void Panda::MemoryManager::Free(void* p, size_t inSize) 
{
	Allocator* pAlloc = LookUpAllocator(inSize);
	
	if (pAlloc)
		pAlloc->Free(p);
	else
		free(p);
}

Panda::Allocator* Panda::MemoryManager::LookUpAllocator(size_t inSize)
{
	if (inSize <= k_MaxBlockSize)
		return m_pAllocators + m_pLookUpTable[inSize];
	else
		return nullptr;
}

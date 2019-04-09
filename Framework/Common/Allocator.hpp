#pragma once

#include <cstddef>
#include <cstdint>

namespace Panda{
	struct BlockHeader {
		BlockHeader* pNext;
	};
	
	struct PageHeader {
		PageHeader* pNext;
		BlockHeader* BlockStart() {
			return reinterpret_cast<BlockHeader*>(this + 1);	// PageHeader之后就是块的起始位置
		}
	};
	
	class Allocator{
	public:
		Allocator();
		Allocator(uint32_t inPageSize, uint32_t inBlockSize, uint32_t inAlignment);
		~Allocator();
	
		void Reset(uint32_t inPageSize, uint32_t inBlockSize, uint32_t inAlignment);
		void* Allocate();
		void Free(void* p);
		void FreeAll();
		
	private:
		BlockHeader* NextBlock(BlockHeader* pCurrentBlock);
		// 禁用拷贝构造函数和赋值操作符
		Allocator(const Allocator& clone);
		Allocator& operator=(const Allocator& rhs);
		
	private:
		uint32_t		m_BlockSize;			// 块的尺寸
		uint32_t		m_PageSize;				// 页的尺寸
		uint32_t		m_BlockCountPerPage;	// 每页的块数
		uint32_t		m_AlignmentSize;		// 对齐尺寸
		
		
		BlockHeader*	m_pFreeBlockList;		// 空余块列表
		uint32_t		m_FreeBlockCount;		// 空余的块数
		uint32_t		m_BlockCount;			// 总块数
		
		PageHeader*		m_pPageList;			// 页列表
		uint32_t		m_PageCount;			// 页数
	};
}
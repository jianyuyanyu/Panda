#pragma once
#include "IRuntimeModule.hpp"
#include "Allocator.hpp"
#include <new>

namespace Panda {
	static const uint32_t k_BlockSizes[] = {
		// 4字节增加
		4,	8,	12,	16,	20,	24,	28,	32,	36,	40,	44,	48,
		52,	56,	60,	64,	68,	72,	76,	80,	84,	88,	92,	96,
		
		// 32字节增加
		128, 160, 192, 224, 256, 288, 320, 352, 384,
		416, 448, 480, 512, 576, 608, 640,
		
		// 64字节增加
		704, 768, 832, 896, 960, 1024
	};
	static const uint32_t k_PageSize = 8192;	// 页尺寸
	static const uint32_t k_Alignment = 4;		// 对齐值
	
	static const uint32_t k_BlockSizeCount = sizeof (k_BlockSizes) / sizeof (k_BlockSizes[0]);	// 预配置的分配器数量
	
	static const uint32_t k_MaxBlockSize = k_BlockSizes[k_BlockSizeCount - 1]; // 预配置的分配器分配的最大内存数
		
	class MemoryManager : implements IRuntimeModule {
	public:
		// C++11的新机制，可变参数模板
		template<typename T, typename... Arguments>
		T* New(Arguments... parameters)
		{
			return new (Allocate(sizeof(T)))T(parameters...);
		}
		
		template<typename T>
		void Delete(T* p)
		{
			p->~T();
			
			Free(reinterpret_cast<void*>(p), sizeof(T));
		}
	public:
		~MemoryManager() {}
		// virtual ~MemoryManager() {}
		
		/* virtual */bool Initialize();
		/* virtual */void Finalize();
		/* virtual */void Tick();
		
		void* Allocate(size_t inSize);
		void Free(void* p, size_t inSize);
		
	private:
		static Allocator* LookUpAllocator(size_t inSize);
		
	private:
		static Allocator* m_pAllocators;
		static uint32_t* m_pLookUpTable;
	}		
}
#include <stddef.h>
#include "MemoryManager.hpp"

namespace Panda
{
    extern MemoryManager* g_pMemoryManager;

    class Buffer
    {
        public:
            Buffer() : m_pData(nullptr), m_Size(0), m_Alignment(alignof(uint32_t)) {}   // alignof:Queries alignment requirements of a type since C++11
            Buffer(size_t size, size_t alignment = 4) : m_Size(size), m_Alignment(alignment) 
            {
                m_pData = reinterpret_cast<uint8_t*> (g_pMemoryManager->Allocate(size, alignment));
            }
            ~Buffer() 
            {
                if (m_pData)
                {
                    g_pMemoryManager->Free(m_pData, m_Size);
                    m_pData = nullptr;
                }
            }
        public:
            uint8_t* m_pData;
            size_t m_Size;
            size_t m_Alignment;
    };
}
#include <iostream>
#include "DebugManager.hpp"
#include "GraphicsManager.hpp"
#include "IPhysicsManager.hpp"

namespace Panda
{
    int DebugManager::Initialize()
    {
        return 0;
    }

    void DebugManager::Finalize()
    {

    }

    void DebugManager::Tick()
    {

    }

    void DebugManager::DrawDebugInfo()
    {
        #ifdef DEBUG
        m_IsDrawDebugInfo = !m_IsDrawDebugInfo;
        if (m_IsDrawDebugInfo)
        {
            DrawDebugInfo();
            g_pPhysicsManager->DrawDebugInfo();
        }
        else 
        {
            g_pGraphicsManager->ClearDebugBuffers();
        }
        #endif
    }
}
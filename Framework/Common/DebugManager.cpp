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
        #ifdef DEBUG
        if (m_IsDrawDebugInfo)
        {
            g_pGraphicsManager->ClearDebugBuffers();
            DrawDebugInfo();
            g_pPhysicsManager->DrawDebugInfo();
        }
        #endif
    }

    void DebugManager::DrawDebugInfo()
    {
        #ifdef DEBUG
        #endif
    }
}
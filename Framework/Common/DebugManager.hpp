#pragma once

#include "IRuntimeModule.hpp"

namespace Panda
{
    class DebugManager : implements IRuntimeModule
    {
        public:
            int Initialize();
            void Finalize();
            void Tick();

            void ToggleDebugInfo();

            void DrawDebugInfo();

        protected:
            bool m_IsDrawDebugInfo = false;
    };

    extern DebugManager* g_pDebugManager;
}
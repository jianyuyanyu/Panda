#pragma once
#include "IGameLogic.hpp"
#include "PandaMath.hpp"
#include "portable.hpp"

namespace Panda
{
    class EditorLogic : implements IGameLogic
    {
        // overrides
        int Initialize() final;
        void Finalize() final;
        void Tick() final;

        void OnLeftKeyDown() final;
        void OnRightKeyDown() final;
        void OnUpKeyDown() final;
        void OnDownKeyDown() final;

        #ifdef DEBUG
        void DrawDebugInfo() final;
        #endif
    };
}
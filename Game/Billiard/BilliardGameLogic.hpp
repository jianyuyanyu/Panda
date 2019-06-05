#pragma once

#include "IGameLogic.hpp"

namespace Panda
{
    class BilliardGameLogic : public IGameLogic
    {
        virtual int Initialize();
        virtual void Finalize();
        virtual void Tick();

        virtual void OnLeftKey();
    };
}
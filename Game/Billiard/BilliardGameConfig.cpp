#include "GfxConfiguration.hpp"
#include "BilliardGameLogic.hpp"
#include "PhysicsManager.hpp"

namespace Panda
{
    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 960, 540, "Billiard Game");
    IGameLogic* g_pGameLogic = static_cast<IGameLogic*>(new BilliardGameLogic);
    IPhysicsManager* g_pPhysicsManager = static_cast<IPhysicsManager*>(new PhysicsManager);
}
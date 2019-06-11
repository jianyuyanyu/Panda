#include "GfxConfiguration.hpp"
#include "EditorLogic.hpp"
#include "PhysicsManager.hpp"

namespace Panda 
{
    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 960, 540, "GameEngineFromScratch Editor");
    IGameLogic*       g_pGameLogic       = static_cast<IGameLogic*>(new EditorLogic);
    IPhysicsManager*  g_pPhysicsManager  = static_cast<IPhysicsManager*>(new PhysicsManager);
}
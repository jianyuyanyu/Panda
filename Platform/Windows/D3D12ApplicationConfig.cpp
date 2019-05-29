#include <tchar.h>
#include "D3D12Application.hpp"
#include "D3D/D3D12GraphicsManager.hpp"

namespace Panda 
{
	// match with articles
	Handness g_ViewHandness = Handness::kHandnessLeft;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;
    
    extern GfxConfiguration config;
	IApplication* g_pApp                = static_cast<IApplication*>(new D3D12Application(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new D3D12GraphicsManager);
    MemoryManager*   g_pMemoryManager   = static_cast<MemoryManager*>(new MemoryManager);
    AssetLoader*     g_pAssetLoader     = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager*    g_pSceneManager    = static_cast<SceneManager*>(new SceneManager);
    InputManager*    g_pInputManager    = static_cast<InputManager*>(new InputManager);
    PhysicsManager*  g_pPhysicsManager  = static_cast<PhysicsManager*>(new PhysicsManager);
}
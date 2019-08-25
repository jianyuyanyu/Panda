#include <tchar.h>
#include "WindowsD3D12Application.hpp"
#include "D3D/D3D12GraphicsManager.hpp"
#include "D3D/D3DShaderModule.hpp"

namespace Panda 
{
	Handness g_ViewHandness = Handness::kHandnessRight;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;
    
    extern GfxConfiguration config;
	IApplication* g_pApp                = static_cast<IApplication*>(new D3D12Application(config));
	IShaderModule*	 g_pShaderModule = static_cast<IShaderModule*>(new D3DShaderModule);
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new D3D12GraphicsManager);
    MemoryManager*   g_pMemoryManager   = static_cast<MemoryManager*>(new MemoryManager);
    AssetLoader*     g_pAssetLoader     = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager*    g_pSceneManager    = static_cast<SceneManager*>(new SceneManager);
    InputManager*    g_pInputManager    = static_cast<InputManager*>(new InputManager);
#ifdef DEBUG
    DebugManager*    g_pDebugManager    = static_cast<DebugManager*>(new DebugManager);
#endif
}
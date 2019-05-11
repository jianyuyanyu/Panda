#include "WindowsApplication.hpp"
#include "D3D/D3D12GraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"
#include <tchar.h>
#include "Utility.hpp"

using namespace Panda;

namespace Panda
{
	// match with articles
	Handness g_ViewHandness = Handness::kHandnessLeft;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;

	GfxConfiguration config(8, 8, 8, 8, 32, 0, 0, 1280, 720, "Panda (Windows D3D12)");
	IApplication* g_pApp				= static_cast<IApplication*>(new WindowsApplication(config));
	GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new D3D12GraphicsManager);
	MemoryManager* g_pMemoryManager 	= static_cast<MemoryManager*>(new MemoryManager);
    AssetLoader*     g_pAssetLoader     = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager*    g_pSceneManager    = static_cast<SceneManager*>(new SceneManager);
}
#include "OpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"

namespace Panda {
	Handness g_ViewHandness = Handness::kHandnessRight;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;

    extern GfxConfiguration config;
    IApplication* g_pApp                = static_cast<IApplication*>(new OpenGLApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
    MemoryManager*   g_pMemoryManager   = static_cast<MemoryManager*>(new MemoryManager);
    AssetLoader*     g_pAssetLoader     = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager*    g_pSceneManager    = static_cast<SceneManager*>(new SceneManager);
    InputManager*    g_pInputManager    = static_cast<InputManager*>(new InputManager);
}
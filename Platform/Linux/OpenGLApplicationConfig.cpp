#include "OpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"

namespace Panda {
	// match with articles
	Handness g_ViewHandness = Handness::kHandnessLeft;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;

    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 960, 540, "Panda (XCB + OpenGL");
    IApplication* g_pApp                = static_cast<IApplication*>(new OpenGLApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
    MemoryManager*   g_pMemoryManager   = static_cast<MemoryManager*>(new MemoryManager);
    AssetLoader*     g_pAssetLoader     = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager*    g_pSceneManager    = static_cast<SceneManager*>(new SceneManager);
    InputManager*    g_pInputManager    = static_cast<InputManager*>(new InputManager);
    PhysicsManager*  g_pPhysicsManager  = static_cast<PhysicsManager*>(new PhysicsManager);
}
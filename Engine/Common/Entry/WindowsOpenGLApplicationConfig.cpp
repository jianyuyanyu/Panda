#include "WindowsOpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "OpenGL/OpenGLShaderModule.hpp"

namespace Panda {
	Handness g_ViewHandness = Handness::kHandnessRight;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;

    extern GfxConfiguration config;
    IApplication* g_pApp                = static_cast<IApplication*>(new OpenGLApplication(config));
	IShaderModule*	 g_pShaderModule	= static_cast<IShaderModule*>(new OpenGLShaderModule);
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
    MemoryManager*   g_pMemoryManager   = static_cast<MemoryManager*>(new MemoryManager);
    AssetLoader*     g_pAssetLoader     = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager*    g_pSceneManager    = static_cast<SceneManager*>(new SceneManager);
    InputManager*    g_pInputManager    = static_cast<InputManager*>(new InputManager);
#ifdef DEBUG
    DebugManager*    g_pDebugManager    = static_cast<DebugManager*>(new DebugManager);
#endif
}
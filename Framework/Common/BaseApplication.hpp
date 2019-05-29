#pragma once
#include "IApplication.hpp"
#include "GraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"
#include "InputManager.hpp"
#include "PhysicsManager.hpp"

namespace Panda {
  class BaseApplication : implements IApplication {
    public:
		BaseApplication(GfxConfiguration& cfg);
		virtual int Initialize();
		virtual void Finalize();

		// 周期调用
		virtual void Tick();

		virtual void SetCommandLineParameters(int argc, char** argv);

		virtual bool IsQuit();
		
		inline GfxConfiguration& GetConfiguration() {return m_Config;}

		virtual int LoadScene();
		virtual void OnDraw() {}

    protected:
		// 是否需要离开主循环的标志
		static bool m_Quit;
		GfxConfiguration m_Config;
		int m_ArgC;
		char** m_ppArgV;
		
	private:
		// 不允许没有配置的构造函数
		BaseApplication() {}
  };

	extern Handness g_EngineHandness; // DO NOT change this. Engine handness is just a showcase.
	extern Handness g_ViewHandness;
	extern DepthClipSpace g_DepthClipSpace;
}

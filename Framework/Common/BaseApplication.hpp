#pragma once
#include "IApplication.hpp"
#include "GfxConfiguration.hpp"

namespace Panda {
  class BaseApplication : implements IApplication {
    public:
		BaseApplication(GfxConfiguration& cfg);
		virtual int Initialize();
		virtual void Finalize();

		// 周期调用
		virtual void Tick();

		virtual bool IsQuit();

    protected:
		// 是否需要离开主循环的标志
		static bool m_Quit;
		GfxConfiguration m_Config;
		
	private:
		// 不允许没有配置的构造函数
		BaseApplication() {}
  };
}

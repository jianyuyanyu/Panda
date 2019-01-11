#pragma once
#include "IApplication.hpp"

namespace Panda {
  class BaseApplication : implements IApplication {
    public:
      virtual int Initialize();
      virtual void Finalize();

      // 周期调用
      virtual void Tick();

      virtual bool IsQuit();

    protected:
      // 是否需要离开主循环的标志
      bool m_bQuit;

  };
}

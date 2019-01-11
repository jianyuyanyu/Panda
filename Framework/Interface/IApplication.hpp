/**
 * 抽象不同平台的Application，并将其模块化
 *
 */
#pragma once
#include "Interface.hpp"
#include "IRuntimeModule.hpp"

namespace Panda {
  Interface IApplication : implements IRuntimeModule {
    public:
      virtual int Initialize() = 0;
      virtual void Finalize() = 0;

      // 主循环的一个周期
      virtual void Tick() = 0;

      virtual bool IsQuit() = 0;
  };
}

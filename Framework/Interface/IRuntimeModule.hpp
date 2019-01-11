#pragma once

#include "Interface.hpp"

namespace Panda {
  Interface IRuntimeModule {
    public:
      virtual ~IRuntimeModule() {};

      virtual int Initialize () = 0;
      virtual void Finalize() = 0;

      virtual void Tick() = 0;
  };
}

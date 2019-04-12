#pragma once

#include "IRuntimeModule.hpp"

namespace Panda {
  class GraphicsManager : implements IRuntimeModule {
    public:
      virtual ~GraphicsManager() {}
	  
	  virtual int Initialize();
	  virtual void Finalize();
	  
	  virtual void Tick();
  };
}

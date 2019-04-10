#pragma once
#include "WindowsApplication.hpp"

namespace Panda
{
	class OpenGLApplication : public WindowsApplication
	{
		public:
			OpenGLApplication(GfxConfiguration& config)
				:WindowsApplication(config) {}
				
			virtual int Initailize();
			virtual void Finalize();
			virtual void Tick();
			
		private:
			HGLRC m_RenderContext;
	};
}
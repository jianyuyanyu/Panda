#pragma once
#include "glad/glad_glx.h"
#include "XcbApplication.hpp"

namespace Panda
{
    class OpenGLApplication : public XcbApplication
    {
        public:
            OpenGLApplication(GfxConfiguration& config)
                : XcbApplication(config) 
            {}

            virtual int Initailize();
            virtual void Finalize();
            virtual void Tick();

        protected:
            virtual void OnDraw();

        private:
            Display*    m_pDisplay;
            GLXContext  m_Context;
            GLXDrawable m_Drawable;
    };
}
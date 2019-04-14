#include <stdio.h>
#include <climits>
#include <cstring>
#include <X11/Xlib-xcb.h>
#include "OpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "MemoryManager.hpp"

using namespace Panda;

namespace Panda
{
    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 1280, 720, "Panda (Ubuntu)");
    IApplication* g_pApp                = static_cast<IApplication*> (new OpenGLApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*> (new OpenGLGraphicsManager);
    MemoryManager* g_pMemoryManager = static_cast<MemoryManager*> (new MemoryManager);
}

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool IsExtensionSupported(const char* extList, const char* extension)
{
    const char* start;
    const char* where;
    const char* terminator;

    // Extension names should not have spaces.
    where = strchr(extension, ' ');
    if (where || *extension == '\0')
        return false;

    // It takes a bit of care to be foll-proof about parsing the 
    // OpenGL extensions string. Don't be fooled by sub-strings, etc.
    for (start = extList;;)
    {
        where = strstr(start, extension);
        
        if (!where)
            break;

        terminator = where + strlen(extension);

        if (where == start | * (where - 1) == ' ')
            if (*terminator == ' ' || *terminator == '\0')
                return true;

        start = terminator;
    }

    return false;
}

static bool ctxErrorOccurred = false;
static int cxtErrorHandler (Display* dpy, XErrorEvent* ev)
{
    ctxErrorOccurred = true;
    return 0;
}

int Panda::OpenGLApplication::Initailize()
{
    int result;

    int defaultScreen;
    GLXFBConfig* fbConfigs;
    GLXFBConfig fbConfig;
    int numFbConfigs = 0;
    XVisualInfo* pVI;
    GLXWindow glxWindow;
    const char* glxExts;

    // Get a matching FB config
    static int virsualAttribs[] =
    {
        GLX_X_RENDERABLE        , True,
        GLX_DRAWABLE_TYPE       , GLX_WINDOW_BIT,
        GLX_RENDER_TYPE         , GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE       , GLX_TRUE_COLOR,
        GLX_RED_SIZE            , static_cast<int>(INT_MAX & m_Config.redBits),
        GLX_GREEN_SIZE          , static_cast<int>(INT_MAX & m_Config.greenBits),
        GLX_BLUE_SIZE           , static_cast<int>(INT_MAX & m_Config.blueBits),
        GLX_ALPHA_SIZE          , static_cast<int>(INT_MAX & m_Config.alphaBits),
        GLX_DEPTH_SIZE          , static_cast<int>(INT_MAX & m_Config.depthBits),
        GLX_STENCIL_SIZE        , static_cast<int>(INT_MAX & m_Config.stencilBits),
        GLX_DOUBLEBUFFER        , True,
        None
    };

    // Open Xlib Display
    m_pDisplay = XOpenDisplay(NULL);
    if (!m_pDisplay)
    {
        fprintf(stderr, "Can't open display\n");
        return -1;
    }

    defaultScreen = DefaultScreen(m_pDisplay);

    gladLoadGLX(m_pDisplay, defaultScreen);

    // Query framebuffer configurations
    fbConfigs = glXChooseFBConfig(m_pDisplay, defaultScreen, virsualAttribs, &numFbConfigs);
    if (!fbConfigs || numFbConfigs == 0)
    {
        fprintf(stderr, "glXGetFBConfigs failed\n");
        return -1;
    }

    
}

void Panda::OpenGLApplication::Finalize()
{
    XcbApplication::Finalize();
}

void Panda::OpenGLApplication::Tick()
{
    XcbApplication::Tick();
}

void Panda::OpenGLApplication::OnDraw()
{
    glXSwapBuffers(m_pDisplay, m_Drawable);
}



#include <stdio.h>
#include <climits>
#include <cstring>

#include <X11/Xlib-xcb.h>

#include "OpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"

using namespace Panda;

namespace Panda
{
    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 1280, 720, "Panda (Ubuntu)");
    IApplication* g_pApp                = static_cast<IApplication*> (new OpenGLApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*> (new OpenGLGraphicsManager);
    MemoryManager* g_pMemoryManager = static_cast<MemoryManager*> (new MemoryManager);
    AssetLoader*     g_pAssetLoader     = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager*    g_pSceneManager    = static_cast<SceneManager*>(new SceneManager);
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

        if (where == start || * (where - 1) == ' ')
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

int Panda::OpenGLApplication::Initialize()
{
    int result;

    int defaultScreen;
    
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

    result = gladLoadGLX(m_pDisplay, defaultScreen);
    if (result == 0)
    {
        std::cout << "gladLoadGLX failed!" << std::endl;
        exit(1);
    }

    // Query framebuffer configurations
    GLXFBConfig* fbConfigs = glXChooseFBConfig(m_pDisplay, defaultScreen, virsualAttribs, &numFbConfigs);
    if (!fbConfigs || numFbConfigs == 0)
    {
        fprintf(stderr, "glXGetFBConfigs failed\n");
        return -1;
    }

    // Pick the FB config/visual with the most samples per pixel
    {
        int bestFbc = -1, worstFbc, bestNumSamp = -1, worstNumSamp = 999;

        for (int i = 0; i < numFbConfigs; ++i)
        {
            XVisualInfo* vi = glXGetVisualFromFBConfig(m_pDisplay, fbConfigs[i]);
            if (vi)
            {
                int sampBuf, samples;
                glXGetFBConfigAttrib(m_pDisplay, fbConfigs[i], GLX_SAMPLE_BUFFERS, &sampBuf);
                glXGetFBConfigAttrib(m_pDisplay, fbConfigs[i], GLX_SAMPLES, &samples);

                printf( " Matching fbconfig %d, visual ID 0x%lx: SAMPLE_BUFFERS = %d,"
                        " SAMPLES = %d\n",
                        i, vi->visualid, sampBuf, samples);

                if (bestFbc < 0 || (sampBuf && samples > bestNumSamp))
                    bestFbc = i, bestNumSamp = samples;
                if (worstFbc < 0 || !sampBuf || samples < worstNumSamp)
                    worstFbc = i, worstNumSamp = samples;
            }
            XFree(vi);
        }

        fbConfig = fbConfigs[bestFbc];
    }

    // Get a visual
    pVI = glXGetVisualFromFBConfig(m_pDisplay, fbConfig);
    printf("Chosen visual ID = 0x%lx\n", pVI->visualid);

    // establish connection to X server
    m_pConn = XGetXCBConnection(m_pDisplay);
    if (!m_pConn)
    {
        XCloseDisplay(m_pDisplay);
        fprintf(stderr, "Can't get xcb connection from display\n");
        return -1;
    }

    // Acquire event queue ownership
    XSetEventQueueOwner(m_pDisplay, XCBOwnsEventQueue);

    // Find XCB screen
    xcb_screen_iterator_t screenIter = xcb_setup_roots_iterator(xcb_get_setup(m_pConn));
    for (int screenNum = pVI->screen; screenIter.rem && screenNum > 0;
        --screenNum, xcb_screen_next(&screenIter));
    m_pScreen = screenIter.data;
    m_Vi = pVI->visualid;

    result = XcbApplication::Initialize();
    if (result)
    {
        printf("Xcb Application initialize fialed!");
        return -1;
    }

    // Get the default screen's GLX extension list
    glxExts = glXQueryExtensionsString(m_pDisplay, defaultScreen);

    // Create OpenGL context
    ctxErrorOccurred = false;
    int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&cxtErrorHandler);

    if (!IsExtensionSupported(glxExts, "GLX_ARB_create_context") ||
        !glXCreateContextAttribsARB)
    {
        printf("glXreateContextAttribsARB() not found"
                "...using old-style GLX context\n");
        m_Context = glXCreateNewContext(
            m_pDisplay,
            fbConfig,
            GLX_RGBA_TYPE,
            0,
            True
        );
        if (!m_Context)
        {
            fprintf(stderr, "glXCreateNewContext failed\n");
            return -1;
        }
    }
    else
    {
        int contextAttribs[] =
        {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        printf("Creating context\n");
        m_Context = glXCreateContextAttribsARB(
            m_pDisplay,
            fbConfig,
            0,
            True,
            contextAttribs
        );
        XSync(m_pDisplay, False);
        if (!ctxErrorOccurred && m_Context)
            printf("Crate GL 3.0 context\n");
        else
        {
            // GLX_CONTEXT_MAJOR_VERSION_ARB = 1
            contextAttribs[1] = 1;
            // GLX_CONTEXT_MINOR_VERSION_ARB = 0
            contextAttribs[3] = 0;

            ctxErrorOccurred = false;

            printf("Failed to create GL 3.0 context"
                    " ... using old-style GLX context\n");
            m_Context = glXCreateContextAttribsARB(
                m_pDisplay, 
                fbConfig,
                0,
                True,
                contextAttribs
            );
        }
    }
    XSync(m_pDisplay, False);

	XSetErrorHandler(oldHandler);

	if (ctxErrorOccurred || !m_Context)
	{
		printf("Failed to create an OpenGL context\n");
		return -1;
	}

	// Verifying that context is a direct context
	if (!glXIsDirect(m_pDisplay, m_Context))
	{
		printf("Idirect GLX rendering context obtained\n");
	}
	else
	{
		printf("Direct GLX rendering context obtained\n");
	}
	
	// Create GLX Window
	glxWindow = glXCreateWindow (
		m_pDisplay,
		fbConfig,
		m_Window,
		0
	);

	if (!glxWindow)
	{
		xcb_destroy_window(m_pConn, m_Window);
		glXDestroyContext(m_pDisplay, m_Context);

		fprintf(stderr, "glxCreateWindow failed\n");
		return -1;
	}

	m_Drawable = glxWindow;

	// make OpenGL context current
	if (!glXMakeContextCurrent(m_pDisplay, m_Drawable, m_Drawable, m_Context))
	{
		xcb_destroy_window(m_pConn, m_Window);
		glXDestroyContext(m_pDisplay, m_Context);

		fprintf(stderr, "glXMakeContextCurrent failed\n");
		return -1;
	}

	XFree(pVI);
	return result;
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (* glXCreateContextAttribsARBProc) (Display*, GLXFBConfig, GLXContext, Bool, const int*);

// Helper to check for extension string presence.  Adapted from:
//   http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension)
{
  const char *start;
  const char *where, *terminator;
  
  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return false;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for (start=extList;;) {
    where = strstr(start, extension);

    if (!where)
      break;

    terminator = where + strlen(extension);

    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return true;

    start = terminator;
  }

  return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display* dpy, XErrorEvent* ev)
{
    ctxErrorOccurred = true;
    return 0;
}

int main (int argc, char* argv[])
{
    // The XOpenDisplay() function returns a Display structure that serves as the connection 
	// to the X server and that contains all the information about that X server.
    Display* display = XOpenDisplay(NULL);  
    if (!display)
    {
        printf("Failed to open X display\n");
        exit(1);
    }

	// FBConfigs were added in GLX version 1.3
    int glx_major, glx_minor;
    if ( !glXQueryVersion( display, &glx_major, &glx_minor ) || 
        ( ( glx_major == 1 ) && ( glx_minor < 3 ) ) || ( glx_major < 1 ) )
    {
      printf("Invalid GLX version");
      exit(1);
    }

    // Get a matching FB config
    // A list of attribute/value pairs.
    static int visual_attribs[] = 
    {
        GLX_X_RENDERABLE,   True,   // If drawables can be renderd to by X.
        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT, // Indicating what drawable types the frame buffer configuration supports. GLX_WINDOW_BIT, GLX_PIXMAP_BIT, GLX_PBUFFER_BIT
        GLX_RENDER_TYPE,    GLX_RGBA_BIT,   // Indicating what type of GLX contexts can be made current to the frame buffer configuration. GLX_RGBA_BIT, GLX_COLOR_INDEX_BIT
        GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR, // Visual type of associated visual.
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,
        GLX_DEPTH_SIZE,     24,
        GLX_STENCIL_SIZE,   8,
        GLX_DOUBLEBUFFER,   True,
        None
    };

    printf( "Getting matching framebuffer configs\n" );
    int fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
    if (!fbc)
    {
      printf( "Failed to retrieve a framebuffer config\n" );
      exit(1);
    }
    printf( "Found %d matching FB configs.\n", fbcount );

    // Pick the FB config/visual with the most samples per pixel
    printf( "Getting XVisualInfos\n" );
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    int i;
    for (i=0; i<fbcount; ++i)
    {
      XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
      if ( vi )
      {
        int samp_buf, samples;
        glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
        glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
        
        printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
                " SAMPLES = %d\n", 
                i, vi -> visualid, samp_buf, samples );

        if ( best_fbc < 0 || samp_buf && samples > best_num_samp )
          best_fbc = i, best_num_samp = samples;
        if ( worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
          worst_fbc = i, worst_num_samp = samples;
      }
      XFree( vi );
    }

    GLXFBConfig bestFbc = fbc[ best_fbc ];

    // Be sure to free the FBConfig list allocated by glXChooseFBConfig()
    XFree( fbc );

    // Get a visual
    XVisualInfo *vi = glXGetVisualFromFBConfig( display, bestFbc );
    printf( "Chosen visual ID = 0x%x\n", vi->visualid );

	printf("Creating colormap\n");
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap(display,
		RootWindow(display, vi->screen),
		vi->visual, AllocNone);
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;

	printf("Creating window\n");
	Window win = XCreateWindow(display, RootWindow(display, vi->screen),
		0, 0, 100, 100, 0, vi->depth, InputOutput,
		vi->visual,
		CWBorderPixel | CWColormap | CWEventMask, &swa);
	if (!win)
	{
		printf("Failed to create window.\n");
		exit(1);
	}

	// Done with the visual info data
	XFree (vi);

	XStoreName(display, win, "GL 3.0 Window");

	printf("Mapping window\n");
	XMapWindow(display, win);

	// Get the default screen's GLX extension list
	const char* glxExts = glXQueryExtensionString(display, DefaultScreen(display));

	// NOTE:It is note necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
		glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	GLXContext ctx = 0;

	// Install an X error handler so the application won't exit if GL 3.0
	// context allocation fails.
	ctxErrorOccurred = false;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&cxtErrorHandler);

	// Check for the GLX_ARB_crate_context extension string and the function.
	// Iff either is not present, use GLX 1.3 context creationg failed.
	if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") ||
		!glXCreateContextAttribsARB)
	{
		printf("glXCreateContextAttribsARB() not found"
				" ... using old-style GLX context\n");
		ctx = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);
	}
	else	// If it does, try to get a GL 3.0 context!
	{
		int context_attribs[] = 
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};

		printf("Creating context\n");
		ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);

		// Sync to ensure any errors generated are processed.
		XSync(display, False);
		if (!ctxErrorOccurred && ctx)
		{
			pritnf("Created GL 3.0 context.\n");
		}
		else
		{
			// Couldn't create GL 3.0 context. Fall back to old-style 2.0 context.
			// When a context version below 3.0 is request,implementations will 
			// return newest context version compatible with OpenGL versions less
			// than version 3.0
			context_attribs[1] = 1;	// GLX_CONTEXT_MAJOR_VERSION_ARB
			context_attribs[3] = 0; // GLX_CONTEXT_MINOR_VERSION_ARB

			ctxErrorOccurred = false;

			printf("Failed to create GL 3.0 context"
					"... using old-style GLX context\n");
			cxt = glXCreateContextAttribsARB(display, bestFbc, 0, Ture, context_attribs);
		}
	}

	// Sync to ensure any errors generated are processed.
	XSync(display, False);

	// Restore the original error handler.
	XSetErrorHandler(oldHandler);

	if (ctxErrorOccurred || !ctx)
	{
		printf("Failed to create an OpenGL context\n");
		exit(1);
	}

	// Verifying that context is a drect context.
	if (!glXIsDirect(display, ctx))
	{
		printf("Indirect GLX redering context obtained.\n");
	}
	else
	{
		printf("Direct GLX rendering context obtained.\n");
	}

	printf("Making context current\n");
	glXMakeCurrent(display, win, ctx);

	glClearColor(0. 0.5, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(display, win);

	sleep(1);

	glClearColor(1, 0.5, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(display, win);

	sleep(1);

	glXMakeCurrent(display, 0, 0);
	glXDestroyContext(display, ctx);

	XDestroyWindow(display, win);
	XFreeColormap(display, cmap);
	XCloseDisplay(display);

	return 0;
}



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


    GLXFBConfig bestFbc = fbc[0];

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
	const char* glxExts = glXQueryExtensionsString(display, DefaultScreen(display));

	// NOTE:It is note necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
		glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	GLXContext ctx = 0;

	int context_attribs[] = 
	{
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		None
	};

	printf("Creating context 3.0\n");
	ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);

	// Sync to ensure any errors generated are processed.
	XSync(display, False);
	if (ctx == 0)
	{
		printf("Can't create GL 3.0 context.\n");
		exit(1);
	}

	// Sync to ensure any errors generated are processed.
	XSync(display, False);

	printf("Draw with context\n");
	glXMakeCurrent(display, win, ctx);

	glClearColor(0, 0.5, 1, 1);
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



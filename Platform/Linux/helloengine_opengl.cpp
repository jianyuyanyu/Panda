#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#define GLX_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB   0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

// 帮助检查扩展字符的存在性
static bool isExtensionSupported(const char* extList, const char* extension) {
  const char* start;
  const char* where, *terminator;
  /* 扩展名不能由空格 */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return false;

  /* 解析OpenGL扩展字符串的时候需要小心一点才能保证安全。
   * 别被那些子字符串给愚弄了。*/
  for (start = extList; ; ) {
    where strstr(start, extension);

    if (!where)
      break;

    terminator = where + strlen (extension);

    if (where == start || *(where - 1) == ' ')
      if (*terminator == ' ' || *terminator == '\0')
        return true;

    start = terminator;
  }

  return false;
}

static bool ctxErrorOccurred = false;
static int cxtErrorHandler (Display* dpy, XErrorEvent* ev) {
  ctxErrorOccurred = true;
  return 0;
}

void DrawAQuad() {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DETH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1., 1., -1., 1., 1., 20.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

  glBegin(GL_QUEDS);
  glColor3f(1., 0., 0.);
  glVertex3f(-.75, -.75, 0.);
  glColor3f(0., 1., 0.);
  glVertex3f(.75, -.75, 0.);
  glColor3f(0., 0., 1.);
  glVertex3f(.75, .75, 0.);
  glColor3f(1., 1., 0.);
  glVertex3f(-.75, .75, 0.);
  glEnd();
}

int main (void) {
  xcb_connection_t*       pConn;
  xcb_screen_t*           pScreen;
  xcb_window_t            window;
  xcb_gcontext_t          foreground;
  xcb_gcontext_t          background;
  xcb_generic_event_t*    pEvent;
  xcb_colormap_t colormap;
  uint32_t                mask = 0;
  uint32_t  values[3];
  uint8_t                 isQuit = 0;

  char title[] = "Hello, Engine![OpenGL]";
  char title_icon[] = "Hello, Engine! (iconified)";

  Display* dislay;
  int default_screen;
  GLXContext context;
  GLXFBConfig* fb_configs;
  GLXFBConfig* fb_config;

  int num_fb_configs = 0;
  XVisualInfo* vi;
  GLXDrawable drawable;
  GLXWindow glxwindow;
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
  const char* glxExts;

  /* 找一个匹配的FB配置 */
  static int visual_attribs[] = {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WIDNOW_BIT,
    GLX_RENDER_TYPE     , GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
    GLX_RED_SIZE        , 8,
    GLX_GREEN_SIZE      , 8,
    GLX_BLUE_SIZE       , 8,
    GLX_ALPHA_SIZE      , 8,
    GLX_DEPTH_SIZE      , 24,
    GLX_STENCIL_SIZE    , 8,
    GLX_DOUBLEBUFFER    , True,
    // GLX_SAMPLE_BUFFERS, 1,
    // GLX_SAMPLES      , 4,
    None
  };

  int glx_major, glx_minor;

  /* 打开Xlib显示 */
  display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "Can't open display\n");
    return -1;
  }

  /* FBConfigs在GLX1.3被加入进去 */
  if (!glXQueryVersion (display, &glx_major, &glx_minor) ||
      ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
    fprintf(stderr, "Invalid GLX version\n");
    return -1;
  }

  default_screen = DefaultScreen(display);


  // 与xServer建立链接
  pConn = xcb_connect(0, 0);

  // 获取第一个屏幕
  pScreen = xcb_setup_roots_iterator(xcb_get_setup(pConn)).data;

  // 获取根窗口
  window = pScreen->root;

  // 创建前端图形上下文
  foreground = xcb_generate_id(pConn);
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = pScreen->black_pixel;
  values[1] = 0;
  xcb_create_gc(pConn, foreground, window, mask, values);

  // 创建后端图形上下文
  background = xcb_generate_id(pConn);
  mask = XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = pScreen->white_pixel;
  values[1] = 0;
  xcb_create_gc(pConn, background, window, mask, values);

  // 创建窗口
  window = xcb_generate_id(pConn);
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = pScreen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
  xcb_create_window(pConn, 
                    XCB_COPY_FROM_PARENT,
                    window,
                    pScreen->root,
                    20, 20,
                    640, 400,
                    10,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT,
                    pScreen->root_visual,
                    mask, values);

  // 设置窗口标题
  xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                      strlen(title), title);

  xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window, 
                      XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8,
                      strlen(title_icon), title_icon);

  // 映射窗口到屏幕上
  xcb_map_window(pConn, window);

  xcb_flush(pConn);

  while ((pEvent = xcb_wait_for_event(pConn)) && !isQuit) {
    switch(pEvent->response_type & ~0x80) {
      case XCB_EXPOSE:
        {
          xcb_rectangle_t rect = {20, 20, 60, 80};
          xcb_poly_fill_rectangle(pConn, window, foreground, 1, &rect);
          xcb_flush(pConn);
        }
        break;
      case XCB_KEY_PRESS:
        isQuit = 1;
        break;
    }
    free(pEvent);
  }

  xcb_disconnect(pConn);

  return 0;
}

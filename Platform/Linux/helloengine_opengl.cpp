#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    where = strstr(start, extension);

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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1., 1., -1., 1., 1., 20.);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

  glBegin(GL_QUADS);
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
  uint32_t                values[3];
  uint8_t                 isQuit = 0;

  char title[] = "Hello, Engine![OpenGL]";
  char title_icon[] = "Hello, Engine! (iconified)";

  Display* display;
  int default_screen;
  GLXContext context;
  GLXFBConfig* fb_configs;
  GLXFBConfig fb_config;

  int num_fb_configs = 0;
  XVisualInfo* vi;
  GLXDrawable drawable;
  GLXWindow glxwindow;
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
  const char* glxExts;

  /* 找一个匹配的FB配置 */
  static int visual_attribs[] = {
    GLX_X_RENDERABLE    , True,
    GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
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

  // 查询帧缓存配置
  fb_configs = glXChooseFBConfig(display, default_screen, visual_attribs, &num_fb_configs);
  if (!fb_configs) {
    fprintf(stderr, "fb_configs = nullptr");
    return -1;
  }
  if (!fb_configs || num_fb_configs == 0) {
    fprintf(stderr, "glXGetFBConfigs failed\n");
    return -1;
  }

  /* Pick the FB config/visual with the most samples per pixel */
  {
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

    for (int i = 0; i < num_fb_configs; ++i) {
      XVisualInfo* vi = glXGetVisualFromFBConfig (display, fb_configs[i]);
      if (vi) {
        int samp_buf, samples;
        glXGetFBConfigAttrib(display, fb_configs[i], GLX_SAMPLE_BUFFERS, &samp_buf);
        glXGetFBConfigAttrib(display, fb_configs[i], GLX_SAMPLES, &samples);

        printf(" Matching fbcofnig %d, visual ID 0x%lx: SAMPLE_BUFFERS = %d,"
               " SAMPLES = %d\n",
               i, vi ->visualid, samp_buf, samples);

        if (best_fbc < 0 || (samp_buf && samples > best_num_samp))
          best_fbc = i, best_num_samp = samples;
        if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
          worst_fbc = i, worst_num_samp = samples;
      }
      XFree(vi);
    }

    fb_config = fb_configs[best_fbc];
  }

  /* 获取一个显示 */
  vi = glXGetVisualFromFBConfig (display, fb_config);
  printf("Chosen visual ID = 0x%lx\n", vi->visualid);

  /* 与xServer建立连接 */
  pConn = XGetXCBConnection(display);
  if (!pConn) {
    XCloseDisplay(display);
    fprintf(stderr, "Can't get xcb connection from display\n");
    return -1;
  }
  
  /* 获取事件队列包含关系 */
  XSetEventQueueOwner(display, XCBOwnsEventQueue);

  /* 找到XCB屏幕 */
  xcb_screen_iterator_t screen_iter = 
    xcb_setup_roots_iterator(xcb_get_setup(pConn));
  for (int screen_num = vi ->screen; screen_iter.rem && screen_num > 0;
      --screen_num, xcb_screen_next(&screen_iter));
  pScreen = screen_iter.data;

  /* 获取根窗口 */
  window = pScreen->root;

  /* 为colormap创建XID */
  colormap = xcb_generate_id(pConn);

  xcb_create_colormap(
      pConn,
      XCB_COLORMAP_ALLOC_NONE,
      colormap,
      window,
      vi->visualid);

  /* 创建窗口 */
  window = xcb_generate_id(pConn);
  mask = XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
  values[0] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
  values[1] = colormap;
  values[2] = 0;
  xcb_create_window(pConn, // 连接
                    XCB_COPY_FROM_PARENT, // 深度缓存
                    window, // 窗口ID
                    pScreen->root,  // 父窗口
                    20, 20, // x,y
                    640, 480,// 宽，高
                    10, // 边框宽度
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, // 类
                    vi->visualid, // 显示
                    mask, values);  // 掩码
  XFree(vi);

  /* 设置窗口的标题 */
  xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window,
      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title), title);

  /* 设置窗口图标的标题 */
  xcb_change_property(pConn, XCB_PROP_MODE_REPLACE, window,
      XCB_ATOM_WM_ICON_NAME, XCB_ATOM_STRING, 8,
      strlen(title_icon), title_icon);

  /* 映射窗口到屏幕上 */
  xcb_map_window(pConn, window);

  xcb_flush(pConn);

  /* 获取默认屏幕的GLX扩展列表 */
  glxExts = glXQueryExtensionsString(display, default_screen);

  /* 注意：在调用glXGetProcAddressARB之桥创建当前上下文不是必要操作 */
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
    glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

  /* 创建OpenGL上下文 */
  ctxErrorOccurred = false;
  int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&cxtErrorHandler);

  if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") || 
      !glXCreateContextAttribsARB) {
    printf("glXCreateContextAttribsARB() not found"
        "... using old-style GLX context\n");
    context = glXCreateNewContext(display, fb_config, GLX_RGBA_TYPE, 0, True);
    if (!context) {
      fprintf(stderr, "glXCreateNewContext failed\n");
      return -1;
    }
  }
  else {
    int context_attribs[] = {
      GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
      GLX_CONTEXT_MINOR_VERSION_ARB, 2,
      None
    };

    printf("Creating context\n");
    context = glXCreateContextAttribsARB(display, fb_config, 0, True, context_attribs);

    XSync(display, False);
    if (!ctxErrorOccurred && context)
      printf("Create GL 3.2 context \n");
    else {
      /* GLX_CONTEXT_MAJOR_VERSION_ARB = 1*/
      context_attribs[1] = 1;
      /* GLX_CONTEXT_MINOR_VERSION_ARB = 0*/
      context_attribs[3] = 0;

      ctxErrorOccurred = false;

      printf("Failed to create GL 3.2 context"
             "... using old-style GLX context\n");
      context = glXCreateContextAttribsARB(display, fb_config, 0, True, context_attribs);
    }
  }

  XSync(display, False);

  XSetErrorHandler(oldHandler);

  if (ctxErrorOccurred || !context) {
    printf("Failed to create an OpenGL context\n");
    return -1;
  }

  /* 确保那个上下文是一个直接上下文 */
  if (!glXIsDirect(display, context)) {
  printf("Indirect GLX rendering context obtained\n");
  }
  else {
    printf("Direct GLX rendering context obtained\n");
  }

  /* 创建GLX窗口 */
  glxwindow = glXCreateWindow(
                display,
                fb_config, 
                window,
                0);

  if (!window) {
    xcb_destroy_window(pConn, window);
    glXDestroyContext(display, context);

    fprintf(stderr, "glXDestroyContext failed\n");
    return -1;
  }

  drawable = glxwindow;

  /* 把OpenGL上下为设置成当前 */
  if (!glXMakeContextCurrent (display, drawable, drawable, context))  {
    xcb_destroy_window(pConn, window);
    glXDestroyContext(display, context);

    fprintf(stderr, "glXMakeContextCurrent failed\n");
    return -1;
  }

  while (!isQuit && (pEvent = xcb_wait_for_event(pConn))) {
    switch (pEvent->response_type & ~0x80) {
      case XCB_EXPOSE:
        {
          DrawAQuad();
          glXSwapBuffers(display, drawable);
        }
        break;
      case XCB_KEY_PRESS:
        isQuit = 1;
        break;
    }
    free(pEvent);
  }

  /* 清理 */
  xcb_disconnect(pConn);

  return 0;
}

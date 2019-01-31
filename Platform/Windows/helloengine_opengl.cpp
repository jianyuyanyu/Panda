// 包含windows头文件
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#include <GL/gl.h>
#include <fstream>

#include "math.h"

using namespace std;

/////////////
// DEFINES //
/////////////
#define WGL_DRAW_TO_WINDOW_ARB         0x2001
#define WGL_ACCELERATION_ARB           0x2003
#define WGL_SWAP_METHOD_ARB            0x2007
#define WGL_SUPPORT_OPENGL_ARB         0x2010
#define WGL_DOUBLE_BUFFER_ARB          0x2011
#define WGL_PIXEL_TYPE_ARB             0x2013
#define WGL_COLOR_BITS_ARB             0x2014
#define WGL_DEPTH_BITS_ARB             0x2022
#define WGL_STENCIL_BITS_ARB           0x2023
#define WGL_FULL_ACCELERATION_ARB      0x2027
#define WGL_SWAP_EXCHANGE_ARB          0x2028
#define WGL_TYPE_RGBA_ARB              0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB  0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB  0x2092
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_TEXTURE0                       0x84C0
#define GL_BGRA                           0x80E1
#define GL_ELEMENT_ARRAY_BUFFER           0x8893

//////////////
// TYPEDEFS //
//////////////
typedef BOOL (WINAPI   * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI  * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL (WINAPI   * PFNWGLSWAPINTERVALEXTPROC) (int interval);
typedef void (APIENTRY * PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY * PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRY * PFNGLBUFFERDATAPROC) (GLenum target, ptrdiff_t size, const GLvoid *data, GLenum usage);
typedef void (APIENTRY * PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint(APIENTRY * PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint(APIENTRY * PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY * PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRY * PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (APIENTRY * PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY * PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY * PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef GLint(APIENTRY * PFNGLGETATTRIBLOCATIONPROC) (GLuint program, const char *name);
typedef void (APIENTRY * PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, char *infoLog);
typedef void (APIENTRY * PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, char *infoLog);
typedef void (APIENTRY * PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const char* *string, const GLint *length);
typedef void (APIENTRY * PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
typedef void (APIENTRY * PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const char *name);
typedef GLint(APIENTRY * PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const char *name);
typedef void (APIENTRY * PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY * PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRY * PFNGLGENERATEMIPMAPPROC) (GLenum target);
typedef void (APIENTRY * PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY * PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY * PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);

// 函数指针
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

typedef struct VertexType {
	VectorType position;
	VectorType color;
} VectexType;

HDC g_deviceContext = 0;
HGLRC g_renderingContext = 0;
char g_videoCardDescription[128];

const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

int g_vertexCount, g_indexCount;
unsigned int g_vertexArrayId, g_vertexBufferId, g_indexBufferId;

unsigned int g_vertexShader;
unsigned int g_frameShader;
unsigned int g_shaderProgram;

const char VS_SHADER_SOURCE_FILE[] = "color.vs";
const char PS_SHADER_SOURCE_FILE[] = "color.ps";

float g_positionX = 0, g_positionY = 0, g_positionZ = -10;
float g_rotationX = 0, g_rotationY = 0, g_rotationZ = 0;
float g_worldMatrix[16];
float g_viewMatrix[16];
float g_projectionMatrix[16];

bool InitializeOpenGL (HWND hwnd, int screenWidth, int screenHeight, float screenDepth, float screenNear, bool vsync) {
	int attributeListInt[19];
	int pixelFormat[1];
	unsigned int formatCount;
	int result;
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	int attributeList[5];
	float fieldOfView, screenAspect;
	char* vendorString;
	char* rendererString;
	
	// 获取窗口的设备上下文
	g_deviceContext = GetDC(hwnd);
	if (!g_deviceContext) {
		return false;
	}
	
	// 支持OpenGL渲染
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;
	
	// 支持渲染到窗口
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;
	
	// 支持硬件加速
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;
	
	// 支持24位的颜色
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;
	
	// 支持24位的深度缓存
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;
	
	// 支持双缓存
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;
	
	// 支持前后缓冲区的交换
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;
	
	// 支持RGBA像素类型
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;
	
	// 支持8位的模板缓存
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;
	
	// 查找符合我们要求的一种像素格式
	result = wglChoosePixelFormatARB(g_deviceContext, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if (result != 1) {
		return false;
	}
	
	// 如果显卡支持我们想要的像素格式，我们就用它
	result = SetPixelFormat(g_deviceContext, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1) {
		return false;
	}
	
	// 使用OpenGL4.0版本
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 0;
	
	// 结束属性列表
	attributeList[4] = 0;
	
	// 创建OpenGL4.0的渲染上下文
	g_renderingContext = wglCreateContextAttribsARB(g_deviceContext, 0, attributeList);
	if (g_renderingContext == NULL) {
		return false;
	}
	
	// 激活渲染上下文
	result = wglMakeCurrent(g_deviceContext, g_renderingContext);
	if (result != 1) {
		return false;
	}
	
	// 将深度缓存清理成1
	glClearDepth(1.0f);
	
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	
	// 多边形绕序设置成顺时针（左手坐标系）
	glFrontFace(GL_CW);
	
	// 启用背面消除
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// 将世界转换矩阵初始化成单位矩阵
	BindIdentityMatrix(g_worldMatrix);
	
	// 设置视野以及屏幕宽高比
	fieldOfView = PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;
	
	
	// 创建透视投影矩阵
	BuildPerspectiveFovLHMatrix(g_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
	
	// 获取显卡名
	vendorString = (char*)glGetString(GL_VENDOR);
	rendererString = (char*)glGetString(GL_RENDERER);
	
	// 保存显卡名到全局变量，便于之后的使用
	strcpy_s(g_videoCardDescription, vendorString);
	strcat_s(g_videoCardDescription, " - ");
	strcat_s(g_videoCardDescription, rendererString);
	
	// 根据输入数据开启/关闭垂直同步
	if (vsync) {
		result = wglSwapIntervalEXT(1);
	}
	else {
		result = wglSwapIntervalEXT(0);
	}
	
	// 垂直同步是否正确设置
	if (result != 1) {
		return false;
	}
	
	return true;
}

bool LoadExtensionList()
{
        // 加载需要用到的OpenGL扩展
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
        if(!wglChoosePixelFormatARB)
        {
                return false;
        }

        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        if(!wglCreateContextAttribsARB)
        {
                return false;
        }

        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        if(!wglSwapIntervalEXT)
        {
                return false;
        }

        glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
        if(!glAttachShader)
        {
                return false;
        }

        glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
        if(!glBindBuffer)
        {
                return false;
        }

        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
        if(!glBindVertexArray)
        {
                return false;
        }

        glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
        if(!glBufferData)
        {
                return false;
        }

        glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
        if(!glCompileShader)
        {
                return false;
        }

        glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
        if(!glCreateProgram)
        {
                return false;
        }

        glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
        if(!glCreateShader)
        {
                return false;
        }

        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
        if(!glDeleteBuffers)
        {
                return false;
        }
        
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
        if(!glDeleteProgram)
        {
                return false;
        }

        glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
        if(!glDeleteShader)
        {
                return false;
        }

        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
        if(!glDeleteVertexArrays)
        {
                return false;
        }

        glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
        if(!glDetachShader)
        {
                return false;
        }

        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
        if(!glEnableVertexAttribArray)
        {
                return false;
        }

        glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
        if(!glGenBuffers)
        {
                return false;
        }

        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
        if(!glGenVertexArrays)
        {
                return false;
        }

        glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
        if(!glGetAttribLocation)
        {
                return false;
        }

        glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
        if(!glGetProgramInfoLog)
        {
                return false;
        }

        glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
        if(!glGetProgramiv)
        {
                return false;
        }

        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
        if(!glGetShaderInfoLog)
        {
                return false;
        }

        glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
        if(!glGetShaderiv)
        {
                return false;
        }

        glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
        if(!glLinkProgram)
        {
                return false;
        }

        glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
        if(!glShaderSource)
        {
                return false;
        }

        glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
        if(!glUseProgram)
        {
                return false;
        }

        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
        if(!glVertexAttribPointer)
        {
                return false;
        }

        glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
        if(!glBindAttribLocation)
        {
                return false;
        }

        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
        if(!glGetUniformLocation)
        {
                return false;
        }

        glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
        if(!glUniformMatrix4fv)
        {
                return false;
        }

        glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
        if(!glActiveTexture)
        {
                return false;
        }

        glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
        if(!glUniform1i)
        {
                return false;
        }

        glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
        if(!glGenerateMipmap)
        {
                return false;
        }

        glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
        if(!glDisableVertexAttribArray)
        {
                return false;
        }

        glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
        if(!glUniform3fv)
        {
                return false;
        }

        glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
        if(!glUniform4fv)
        {
                return false;
        }

        return true;
}

void FinalizeOpenGL(HWND hwnd) {
	// 释放渲染上下文
	if (g_renderingContext) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_renderingContext);
		g_renderingContext = 0;
	}
	
	// 释放设备上下文
	if (g_deviceContext) {
		ReleaseDC(hwnd, g_deviceContext);
		g_deviceContext = 0;
	}
}

void GetVideoCardInfo(char* cardName) {
	strcpy_s(cardName, 128, g_videoCardDescription);
	return;
}

bool InitializeExtensions(HWND hwnd) {
	HDC deviceContext;
	PIXELFORMATDESCRIPTOR pixelFormat;
	int error;
	HGLRC renderContext;
	bool result;
	
	// 获取窗口的设备上下文
	deviceContext = GetDC(hwnd);
	if (!deviceContext) {
		return false;
	}
	
	// 设置当前默认像素格式
	error = SetPixelFormat(deviceContext, 1, &pixelFormat);
	if (error != 1) {
		return false;
	}
	
	// 创建当前渲染上下文
	renderContext = wglCreateContext(deviceContext);
	if (!renderContext) {
		return false;
	}
	
	// 将创建的渲染上下文赋予窗口
	error = wglMakeCurrent(deviceContext, renderContext);
	if (error != 1) {
		return false;
	}
	
	// 初始化OpenGL扩展
	result = LoadExtensionList();
	if (!result) {
		return false;
	}
	
	// 既然扩展已经被加载了，那就可以删除当前渲染上下文了
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(renderContext);
	renderContext = NULL;
	
	// 释放设备上下文
	ReleaseDC(hwnd, deviceContext);
	deviceContext = 0;
	
	return true;
}

void OutputShaderErrorMessage(HWND hwnd, unsigned int shaderId, const char* shaderFilename) {
	int logSize, i;
	char* infoLog;
	ofstream fout;
	wchar_t newString[128];
	unsigned int error;
	size_t convertedChars;
	
	// 获取编译出错的信息
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);
	
	// 最后的结束符还需要一个字节
	logSize++;
	
	// 创建字符缓冲区保存信息
	infoLog = new char[logSize];
	if (!infoLog) {
		return;
	}
	
	// 检查信息
	glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);
	
	// 打开文件写入错误信息
	fout.open("shader-error.txt");
	
	// 写入错误信息
	for (i = 0; i < logSize; ++i) {
		fout << infoLog[i];
	}
	
	// 关闭文件
	fout.close();
	
	// 转换着色器文件成宽字符串
	error = mbstowcs_s(&convertedChars, newString, 128, shaderFilename, 128);
	if (error != 0) {
		return;
	}
	
	// 弹出提示
	MessageBoxW(hwnd, L"Error compiling shader. Check shader-error.txt for message.", newString, MB_OK);
	
	return;
}

// WindowProc函数原型
LRESULT CALLBACK WindowProc(HWND hWnd,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

// Windows应用的入口函数
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
  // 窗口句柄，由函数生成
    HWND hWnd;
    // 保存窗口类的信息
    WNDCLASSEX wc;

    // 初始化数据
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // 使用需要的数据填充类结构
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1");

    // 注册窗口类
    RegisterClassEx(&wc);

    // 创建窗口，保存句柄
    hWnd = CreateWindowEx(0,
                          _T("WindowClass1"),    // 窗口名
                          _T("Hello, Engine!"),   // 窗口标题
                          WS_OVERLAPPEDWINDOW,    // 窗口格式
                          300,    // 窗口x坐标
                          300,    // 窗口y坐标
                          500,    //窗口宽度
                          400,    // 窗口高度
                          NULL,    // 父窗口
                          NULL,    // 菜单项
                          hInstance,    // 实例句柄
                          NULL);    // 多窗口应用

    // 显示窗口
    ShowWindow(hWnd, nCmdShow);

    // 进入主循环

    // 这个结构提保存了Windows事件消息
    MSG msg;

    // 等待下一个消息，保存到msg结构中
    while(GetMessage(&msg, NULL, 0, 0))
    {
        // 转换按键消息到合适的结构
        TranslateMessage(&msg);

        // 发送消息到WindowProc函数
        DispatchMessage(&msg);
    }

    // 返回WM_QUIT的部分内容给Windows
    return msg.wParam;
}

// 这是消息处理函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 排序，找到合适的处理过程
    switch(message)
    {
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			
			HDC hdc = BeginPaint(hWnd, &ps);
			RECT rect = {20, 20, 60, 80};
			HBRUSH brush = (HBRUSH)GetStockObject(BLACK_BRUSH);
			
			FillRect(hdc, &rect, brush);
			
			EndPaint(hWnd, &ps);
		}
			break;
			
        // 窗口关闭的时候这个消息会处理
        case WM_DESTROY:
            {
                // 关闭整个应用
                PostQuitMessage(0);
                return 0;
            } 
			break;
    }

    // 处理剩余的消息
    return DefWindowProc (hWnd, message, wParam, lParam);
}


#include <stdio.h>
#include <tchar.h>
#include "OpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"
#include "glad/glad_wgl.h"
#include "Utility.hpp"

using namespace Panda;

namespace Panda
{
	// match with articles
	Handness g_ViewHandness = Handness::kHandnessLeft;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipZeroToOne;

	GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 1280, 720, "Panda (Windows OpenGL)");
	IApplication* g_pApp = static_cast<IApplication*>(new OpenGLApplication(config));
	GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
	MemoryManager* g_pMemoryManager = static_cast<MemoryManager*> (new MemoryManager);
	AssetLoader* g_pAssetLoader = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager* g_pSceneManager = static_cast<SceneManager*>(new SceneManager);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}

	return 0;
}

int Panda::OpenGLApplication::Initialize()
{
	int result;
	auto colorBits = m_Config.redBits + m_Config.greenBits + m_Config.blueBits;

	// create a temporary window for OpenGL context loading
	DWORD Style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	WNDCLASSEX WndClassEx;
	memset(&WndClassEx, 0, sizeof(WNDCLASSEX));

	HINSTANCE hInstance = GetModuleHandle(NULL);

	WndClassEx.cbSize = sizeof(WNDCLASSEX);
	WndClassEx.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	WndClassEx.lpfnWndProc = WndProc;
	WndClassEx.hInstance = hInstance;
	WndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.lpszClassName = _T("InitWindow");

	RegisterClassEx(&WndClassEx);
	HWND TemphWnd = CreateWindowEx(WS_EX_APPWINDOW, WndClassEx.lpszClassName, _T("InitWindow"), Style, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = colorBits;
	pfd.cAlphaBits = m_Config.alphaBits;
	pfd.cDepthBits = m_Config.depthBits;
	pfd.cStencilBits = m_Config.stencilBits;
	pfd.iLayerType = PFD_MAIN_PLANE;
	
	HDC TemphDC = GetDC(TemphWnd);
	// Set a temporary default pixel format.
	int pixelFormat = ChoosePixelFormat(TemphDC, &pfd);
	if (pixelFormat == 0)
	{
		int error = GetLastError();
		printf("Windows could not support the pixel format. ErrorCode = %d\n", error);
		return -1;
	}
	
	result = SetPixelFormat(TemphDC, pixelFormat, &pfd);
	if (result != 1)
	{
		int error = GetLastError();
		printf("Windows could not set the pixel format. ErrorCode = %d\n", error);
		return result;
	}
	
	// Crete a temporary rendering context.
	m_RenderContext = wglCreateContext(TemphDC);
	if (!m_RenderContext)
	{
		int error = GetLastError();
		printf("Windows could not create the context. ErrorCode = %d\n", error);
		return -1;
	}

	// Set the temporary rendering context as the current rendering context for this window.
	result = wglMakeCurrent(TemphDC, m_RenderContext);
	if (result != 1)
	{
		int error = GetLastError();
		printf("Windows could not make current context. ErrorCode = %d\n", error);
		return result;
	}
	
	if (!gladLoadWGL(TemphDC))
	{
		printf("WGL initialize failed!\n");
		result = -1;
	}
	else
	{
		result = 0;
		printf("WGL initialize finished!\n");
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_RenderContext);
	ReleaseDC(TemphWnd, TemphDC);
	DestroyWindow(TemphWnd);

	result = WindowsApplication::Initialize();
	if (result)
	{
		printf("Windows Application initialize faield!");
		return result;
	}

	m_hDC = GetDC(m_hWnd);

	// now we try to init OpenGL core profile context
	if (GLAD_WGL_ARB_pixel_format && GLAD_WGL_ARB_multisample && GLAD_WGL_ARB_create_context)
	{
		// enable MSAA
		const int attributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, 	GL_TRUE,
			WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,		colorBits,
			WGL_ALPHA_BITS_ARB,		m_Config.alphaBits,
			WGL_DEPTH_BITS_ARB,		m_Config.depthBits,
			WGL_STENCIL_BITS_ARB,	m_Config.stencilBits,
			WGL_SAMPLE_BUFFERS_ARB,	1,
			WGL_SAMPLES_ARB,		4, // 4x MSAA
			0
		};

		UINT numFormats;

		if (FAILED(wglChoosePixelFormatARB(m_hDC, attributes, nullptr, 1, &pixelFormat, &numFormats)) || numFormats == 0)
		{
			printf("wglChoosePixelFormatARB failed!\n");
			return -1;
		}

		result = SetPixelFormat(m_hDC, pixelFormat, &pfd);
		if (result != 1)
		{
			return result;
		}

		const int context_attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB,	3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 	2,
			WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		m_RenderContext = wglCreateContextAttribsARB(m_hDC, 0, context_attributes);
		if (!m_RenderContext)
		{
			printf("wglCreateContextAttributeARB failed!\n");
			return -1;
		}

		result = wglMakeCurrent(m_hDC, m_RenderContext);
		if (result != 1)
		{
			return result;
		}

		return 0; // we use 0 as success while OpenGL use 1, so convert it
	}
	else
	{
		// Set pixel format
		int nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
		if (nPixelFormat == 0)
			return -1;

		result = SetPixelFormat(m_hDC, nPixelFormat, &pfd);
		if (result != 1)
		{
			return result;
		}

		// Create rendering context.
		m_RenderContext = wglCreateContext(m_hDC);
		if (!m_RenderContext)
		{
			printf("wglCreateContext failed!\n");
			return -1;
		}

		// Set the rendering context as the current rendering context for this window.
		result = wglMakeCurrent(m_hDC, m_RenderContext);
		if (result != 1)
		{
			return result;
		}

		result = 0;
	}

	return result;
	
	return result;
}

void Panda::OpenGLApplication::Finalize()
{
	if (m_RenderContext)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_RenderContext);
		m_RenderContext = 0;
	}
	
	WindowsApplication::Finalize();
}

void Panda::OpenGLApplication::Tick()
{
	WindowsApplication::Tick();

	g_pGraphicsManager->Clear();
	g_pGraphicsManager->Draw();

	// Present the back buffer to the screen since rendering is complete
	SwapBuffers(m_hDC);
}

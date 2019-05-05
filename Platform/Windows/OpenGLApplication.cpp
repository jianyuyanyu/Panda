#include <stdio.h>
#include <tchar.h>
#include "OpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "glad/glad_wgl.h"

using namespace Panda;

namespace Panda
{
	GfxConfiguration config(8, 8, 8, 8, 24, 0, 0, 1280, 720, "Panda (Windows OpenGL)");
	IApplication* g_pApp = static_cast<IApplication*>(new OpenGLApplication(config));
	GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
	MemoryManager* g_pMemoryManager = static_cast<MemoryManager*> (new MemoryManager);
}

int Panda::OpenGLApplication::Initialize()
{
	int result;
	result = WindowsApplication::Initialize();
	if (result)
	{
		printf("Windows Application initialize fialed!\n");
	}
	else
	{
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = m_Config.redBits + m_Config.greenBits + m_Config.blueBits + m_Config.alphaBits;
		pfd.cDepthBits = m_Config.depthBits;
		pfd.iLayerType = PFD_MAIN_PLANE;
		
		HWND hWnd = reinterpret_cast<WindowsApplication*>(g_pApp)->GetMainWindow();
		HDC hDC = GetDC(hWnd);
		// Set a temporary default pixel format.
		int pixelFormat = ChoosePixelFormat(hDC, &pfd);
		if (pixelFormat == 0)
		{
			int error = GetLastError();
			printf("Windows could not support the pixel format. ErrorCode = %d\n", error);
			return -1;
		}
		
		result = SetPixelFormat(hDC, pixelFormat, &pfd);
		if (result != 1)
		{
			int error = GetLastError();
			printf("Windows could not set the pixel format. ErrorCode = %d\n", error);
			return -1;
		}
		
		// Crete a temporary rendering context.
		m_RenderContext = wglCreateContext(hDC);
		if (!m_RenderContext)
		{
			int error = GetLastError();
			printf("Windows could not create the context. ErrorCode = %d\n", error);
			return -1;
		}
		
		// Set the temporary rendering context as the current rendering context for this window.
		result = wglMakeCurrent(hDC, m_RenderContext);
		if (result != 1)
		{
			int error = GetLastError();
			printf("Windows could not make current context. ErrorCode = %d\n", error);
			return -1;
		}
		
		if (!gladLoadWGL(hDC))
		{
			printf("WGL initialize failed!\n");
			result = -1;
		}
		else
		{
			result = 0;
			printf("WGL initialize finished!\n");
		}
	}
	
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
}

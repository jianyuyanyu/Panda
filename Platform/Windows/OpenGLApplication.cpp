#include <stdio.h>
#include <tchar.h>
#include "OpenGLApplication.hpp"
#include "OpenGL/OpenGLGraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"
#include "glad/glad_wgl.h"

using namespace Panda;

namespace Panda
{
	GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 1280, 720, "Panda (Windows OpenGL)");
	IApplication* g_pApp = static_cast<IApplication*>(new OpenGLApplication(config));
	GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new OpenGLGraphicsManager);
	MemoryManager* g_pMemoryManager = static_cast<MemoryManager*> (new MemoryManager);
	AssetLoader* g_pAssetLoader = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager* g_pSceneManager = static_cast<SceneManager*>(new SceneManager);
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
		
		m_hDC = GetDC(m_hWnd);
		// Set a temporary default pixel format.
		int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
		if (pixelFormat == 0)
		{
			int error = GetLastError();
			printf("Windows could not support the pixel format. ErrorCode = %d\n", error);
			return -1;
		}
		
		result = SetPixelFormat(m_hDC, pixelFormat, &pfd);
		if (result != 1)
		{
			int error = GetLastError();
			printf("Windows could not set the pixel format. ErrorCode = %d\n", error);
			return -1;
		}
		
		// Crete a temporary rendering context.
		m_RenderContext = wglCreateContext(m_hDC);
		if (!m_RenderContext)
		{
			int error = GetLastError();
			printf("Windows could not create the context. ErrorCode = %d\n", error);
			return -1;
		}

		// Set the temporary rendering context as the current rendering context for this window.
		result = wglMakeCurrent(m_hDC, m_RenderContext);
		if (result != 1)
		{
			int error = GetLastError();
			printf("Windows could not make current context. ErrorCode = %d\n", error);
			return -1;
		}
		
		if (!gladLoadWGL(m_hDC))
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

	g_pGraphicsManager->Clear();
	g_pGraphicsManager->Draw();

	// Present the back buffer to the screen since rendering is complete
	SwapBuffers(m_hDC);
}

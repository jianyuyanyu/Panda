#include "D3D12Application.hpp"
#include <tchar.h>

namespace Panda
{
	void D3D12Application::Tick()
	{
		WindowsApplication::Tick();
		g_pGraphicsManager->Clear();
		g_pGraphicsManager->Draw();

		// Present the back buffer to the screen since rendering is complete.
		HDC hdc = GetDC(m_hWnd);
		SwapBuffers(hdc);
		ReleaseDC(m_hWnd, hdc);
	}
}
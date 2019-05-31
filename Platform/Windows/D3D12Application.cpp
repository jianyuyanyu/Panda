#include <tchar.h>
#include "D3D12Application.hpp"
#include "D3D/D3D12GraphicsManager.hpp"

namespace Panda
{
	void D3D12Application::Tick()
	{
		WindowsApplication::Tick();
		// Present the back buffer to the screen since rendering is complete.
		SwapBuffers(m_hDC);
	}
}
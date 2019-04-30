#include "WindowsApplication.hpp"
#include "D3D/D3D12GraphicsManager.hpp"
#include "MemoryManager.hpp"
#include <tchar.h>

using namespace Panda;

namespace Panda
{
	GfxConfiguration config(8, 8, 8, 8, 32, 0, 0, 1280, 720, "Panda (Windows D3D12)");
	IApplication* g_pApp		= static_cast<IApplication*>(new WindowsApplication(config));
	GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new D3D12GraphicsManager);
	MemoryManager* g_pMemoryManger = static_cast<MemoryManager*>(new MemoryManager);
}
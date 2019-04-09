#include "BaseApplication.hpp"

namespace Panda {
	GfxConfiguration config;
	BaseApplication g_App(config);
    IApplication* g_pApp = &g_App;
}

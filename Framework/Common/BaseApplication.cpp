#include "BaseApplication.hpp"

bool Panda::BaseApplication::m_Quit = false;

Panda::BaseApplication::BaseApplication(GfxConfiguration& cfg)
	: m_Config(cfg)
{
	
}

int Panda::BaseApplication::Initialize() {
	std::cout << m_Config;
	return 0;
}


void Panda::BaseApplication::Finalize() {
  
}


void Panda::BaseApplication::Tick() {
}

void Panda::BaseApplication::SetCommandLineParameters(int argc, char** argv)
{
	m_ArgC = argc;
	m_ppArgV = argv;
}

bool Panda::BaseApplication::IsQuit() {
  return m_Quit;
}

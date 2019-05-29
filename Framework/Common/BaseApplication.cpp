#include "BaseApplication.hpp"

namespace Panda
{
	bool BaseApplication::m_Quit = false;

	BaseApplication::BaseApplication(GfxConfiguration& cfg)
		: m_Config(cfg)
	{
		
	}

	int BaseApplication::Initialize() {
		int ret = 0;
		std::cout << m_Config;

		std::cerr << "Initialize Memory Manager:";
		if((ret = g_pMemoryManager->Initialize()) != 0)
		{
			std::cerr << "Fialed. err = " << ret;
			return ret;
		}
		std::cerr << "Success" << std::endl;

		std::cerr << "Initialize Asset Loader: ";
		if ((ret = g_pAssetLoader->Initialize() != 0) != 0)
		{
			std::cerr << "Failed. err = " << ret;
			return ret;
		}
		std::cerr << "Success" << std::endl;

		std::cerr << "Initialize Scene Manager:";
		if ((ret = g_pSceneManager->Initialize()) != 0)
		{
			std::cerr << "Failed. err = " << ret;
			return ret;
		}
		std::cerr << "Success" << std::endl;

		std::cerr << "Initialize Graphics Manager:";
		if ((ret = g_pGraphicsManager->Initialize()) != 0)
		{
			std::cerr << "Failed. err = " << ret;
			return ret;
		}
		std::cerr << "Success" << std::endl;

		std::cerr << "Initialize Input Manager:";
		if ((ret = g_pInputManager->Initialize()) != 0)
		{
			std::cerr << "Failed. err = " << ret;
			return ret;
		}
		std::cerr << "Success" << std::endl;

		std::cerr << "Initialize Physics Manager:";
		if ((ret = g_pPhysicsManager->Initialize()) != 0)
		{
			std::cerr << "Failed. err = " << ret;
			return ret;
		}
		std::cerr << "Success" << std::endl;

		return ret;
	}


	void BaseApplication::Finalize() {
		g_pInputManager->Finalize();
		g_pGraphicsManager->Finalize();
		g_pPhysicsManager->Finalize();
		g_pSceneManager->Finalize();
		g_pAssetLoader->Finalize();
		g_pMemoryManager->Finalize();
	}


	void BaseApplication::Tick() {
		g_pMemoryManager->Tick();
		g_pAssetLoader->Tick();
		g_pSceneManager->Tick();
		g_pInputManager->Tick();
		g_pPhysicsManager->Tick();
		g_pGraphicsManager->Tick();
	}

	void BaseApplication::SetCommandLineParameters(int argc, char** argv)
	{
		m_ArgC = argc;
		m_ppArgV = argv;
	}

	bool BaseApplication::IsQuit() {
		return m_Quit;
	}

	int BaseApplication::LoadScene()
	{
		int ret;

		std::string sceneFileName = "Scene/test.ogex";
		if (m_ArgC > 1)
		{
			sceneFileName = m_ppArgV[1];
		}

		std::cerr << "Load Scene(" << sceneFileName << "):";
		if ((ret = g_pSceneManager->LoadScene(sceneFileName.c_str())) != 0)
		{
			std::cerr << "Failed. err = " << ret;
			return ret;
		}
		std::cerr << "Success" << std::endl;

		return 0;
	}
}


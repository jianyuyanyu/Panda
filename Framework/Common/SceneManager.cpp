#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "OGEX.hpp"

namespace Panda
{
	SceneManager::~SceneManager()
	{

	}

	int SceneManager::Initialize()
	{
		int result = 0;

		m_pScene = std::make_shared<Scene>();
		return result;
	}

	void SceneManager::Finalize()
	{

	}

	void SceneManager::Tick()
	{

	}

	int SceneManager::LoadScene(const char* sceneFileName)
	{
		// now we only have ogex scene parser, call it directly
		if (LoadOgexScene(sceneFileName)) 
		{
			m_pScene->LoadResource();
			m_DirtyFlag = true;
			return 0;
		}
		else
		{
			return -1;
		}
	}

	void SceneManager::ResetScene()
	{
		m_DirtyFlag = true;
	}

	bool SceneManager::LoadOgexScene(const char* ogexSceneFileName)
	{
		std::string ogexText = g_pAssetLoader->SyncOpenAndReadFileToString(ogexSceneFileName);

		if (ogexText.empty())
		{
			return false;
		}
		OgexParser ogexParser;
		m_pScene = ogexParser.Parse(ogexText);

		if (!m_pScene)
		{
			return false;
		}

		return true;
	}

	const Scene& SceneManager::GetScene()
	{
		return *m_pScene;
	}

	bool SceneManager::IsSceneChanged()
	{
		return m_DirtyFlag;
	}

	void SceneManager::NotifySceneIsRenderingQueued()
	{
		m_DirtyFlag = false;
	}
}

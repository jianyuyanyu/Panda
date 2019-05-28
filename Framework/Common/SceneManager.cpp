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

		m_pScene = std::make_unique<Scene>();
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
		m_DirtyFlag = false;
		return *m_pScene;
	}

	bool SceneManager::IsSceneChanged()
	{
		return m_DirtyFlag;
	}
}

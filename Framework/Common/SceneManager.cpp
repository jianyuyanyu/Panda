#include "SceneManager.hpp"
#include "AssetLoader.hpp"
#include "OGEX.hpp"

using namespace Panda;

SceneManager::~SceneManager()
{

}

int SceneManager::Initialize()
{
    int result = 0;
    return result;
}

void SceneManager::Finalize()
{

}

void SceneManager::Tick()
{

}

void SceneManager::LoadScene(const char* sceneFileName)
{
    // now we only have ogex scene parser, call it directly
    LoadOgexScene(sceneFileName);
}

void SceneManager::LoadOgexScene(const char* ogexSceneFileName)
{
    std::string ogexText = g_pAssetLoader->SyncOpenAndReadFileToString(ogexSceneFileName);

    OgexParser ogexParser;
    m_pScene = ogexParser.Parse(ogexText);
}

const Scene& SceneManager::GetScene()
{
    return *m_pScene;
}

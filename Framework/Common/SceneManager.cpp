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

void SceneManager::LoadOgexScene(const char* sceneFileName)
{
    AssetLoader assetLoader;
    std::string ogexText = assetLoader.SyncOpenAndReadFileToString(sceneFileName);

    OgexParser ogexParser;
    m_RootNode = ogexParser.Parse(ogexText);
}

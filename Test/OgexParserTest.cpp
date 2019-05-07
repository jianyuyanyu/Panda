#include <iostream>
#include <string>
#include "AssetLoader.hpp"
#include "MemoryManager.hpp"
#include "OGEX.hpp"

using namespace Panda;

namespace Panda
{
    MemoryManager* g_pMemoryManager = new MemoryManager();
}

int main (int , char**)
{
    g_pMemoryManager->Initialize();

    AssetLoader asset_loader;
    string ogex_text = asset_loader.SyncOpenAndReadFileToString("Scene/Example.ogex");

    OgexParser* ogex_parser = new OgexParser();
    unique_ptr<BaseSceneNode> root_node = ogex_parser->Parse(ogex_text);
    delete ogex_parser;

    std::cout << *root_node << std::endl;

    g_pMemoryManager->Finalize();

    delete g_pMemoryManager;

	getchar();
    
    return 0;
}
#include <iostream>
#include <string>
#include "AssetLoader.hpp"
#include "MemoryManager.hpp"

using namespace std;
using namespace Panda;

namespace Panda
{
    MemoryManager* g_pMemoryManager = new MemoryManager();
}

int main (int argc, char** argv)
{
    g_pMemoryManager->Initialize();

    AssetLoader assetLoader;
    string str = assetLoader.SyncOpenAndReadFileToString("Shaders/copy.vs");
	Buffer buffer = assetLoader.SyncOpenAndReadBinary("Shaders/copy.vs");

    cout << str;

    g_pMemoryManager->Finalize();

    delete g_pMemoryManager;

	getchar();
    return 0;
}
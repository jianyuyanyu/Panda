#include <iostream>
#include <string>
#include "AssetLoader.hpp"
#include "MemoryManager.hpp"
#include "SceneManager.hpp"

using namespace Panda;
using namespace std;

namespace Panda
{
	Handness g_ViewHandness = Handness::kHandnessRight;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipNegativeOneToOne;

    MemoryManager* g_pMemoryManager = new MemoryManager();
    AssetLoader* g_pAssetLoader = new AssetLoader();
    SceneManager* g_pSceneManager = new SceneManager();
}

template <typename T>
static ostream& operator<< (ostream& out, unordered_map<string, shared_ptr<T>> map)
{
    for (auto p : map)
        out << *p.second << endl;

    return out;
}

int main(int, char**)
{
    g_pMemoryManager->Initialize();
    g_pSceneManager->Initialize();
    g_pAssetLoader->Initialize();

    g_pSceneManager->LoadScene("Scene/Example.ogex");
    auto& scene = g_pSceneManager->GetScene();

    cout << "Dump of Cameras" << endl;
    cout << "----------------------" << endl;
	weak_ptr<SceneCameraNode> pCameraNode = scene.GetFirstCameraNode();
    while (auto pObj = pCameraNode.lock())
    {
		auto pCamera = scene.GetCamera(pObj->GetSceneObjectRef());
		if (pCamera.get() == nullptr)
			break;
        cout << *pCamera << endl;
		pCameraNode = scene.GetNextCameraNode();
    }

    cout << "Dump of Lights" << endl;
    cout << "-----------------------" << endl;
	weak_ptr<SceneLightNode> pLightNode = scene.GetFirstLightNode();
    while (auto pObj = pLightNode.lock())
    {
		auto pLight = scene.GetLight(pObj->GetSceneObjectRef());
		if (pLight.get() == nullptr)
			break;
        cout << *pLight <<endl;
		pLightNode = scene.GetNextLightNode();
    }

    cout << "Dump of Geometries" << endl;
    cout << "-----------------------" << endl;
	weak_ptr<SceneGeometryNode> pGeometryNode = scene.GetFirstGeometryNode();
    while (auto pObj = pGeometryNode.lock())
    {
		auto pGeometry = scene.GetGeometry(pObj->GetSceneObjectRef());
		if (pGeometry.get() == nullptr)
			break;
        cout << *pGeometry << endl;
		pGeometryNode = scene.GetNextGeometryNode();
    }

    cout << "Dump of metarials" << endl;
    cout << "------------------------" << endl;
    weak_ptr<SceneObjectMaterial> pMaterial = scene.GetFirstMaterial();
    while(auto pObj = pMaterial.lock())
    {
        cout << *pObj << endl;
        pMaterial = scene.GetNextMaterial();
    }

    g_pSceneManager->Finalize();
    g_pAssetLoader->Finalize();
    g_pMemoryManager->Finalize();

    delete g_pSceneManager;
    delete g_pAssetLoader;
    delete g_pMemoryManager;

    getchar();
    return 0;
}

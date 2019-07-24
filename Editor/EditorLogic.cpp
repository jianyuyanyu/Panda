#include "EditorLogic.hpp"
#include "Interface/IApplication.hpp"
#include "SceneManager.hpp"

using namespace Panda;
using namespace std;

int EditorLogic::Initialize()
{
    int result;

    std::cout << "[Editor Logic] Editor Logic Initialize" << std::endl;

    // if (g_pApp->GetCommandLineArgumentsCount() > 1)
    // {
    //     auto scene_filename = g_pApp->GetCommandLineArgument(1);
    //     cout << "[EditorLogic] Loading Scene: " << scene_filename << endl;
    //     result = g_pSceneManager->LoadScene(scene_filename);
    // }
    // else
    // {
        cout << "[EditorLogic] Loading Splash Scene" << endl;
		// donuts.ogex
        //result = g_pSceneManager->LoadScene("Scene/cube.ogex");
		result = g_pSceneManager->LoadScene("Scene/donuts.ogex");
    //}

	return 0;
}

void EditorLogic::Finalize()
{
}

void EditorLogic::Tick()
{
}

#ifdef DEBUG
void EditorLogic::DrawDebugInfo()
{
}
#endif

void EditorLogic::OnLeftKeyDown()
{
    auto& scene = g_pSceneManager->GetScene();
    auto pCameraNode = scene.GetFirstCameraNode();
    if (pCameraNode) {
        auto local_axis = pCameraNode->GetLocalAxis(); 
        Vector3Df camera_x_axis;
        memcpy(camera_x_axis.data, local_axis.m[0], sizeof(camera_x_axis));

        // move camera along its local axis x direction
        pCameraNode->MoveBy(camera_x_axis);
    }
}

void EditorLogic::OnRightKeyDown()
{
    auto& scene = g_pSceneManager->GetScene();
    auto pCameraNode = scene.GetFirstCameraNode();
    if (pCameraNode) {
        auto local_axis = pCameraNode->GetLocalAxis(); 
        Vector3Df camera_x_axis;
        memcpy(camera_x_axis.data, local_axis.m[0], sizeof(camera_x_axis));

        // move along camera local axis -x direction
        pCameraNode->MoveBy(camera_x_axis * -1.0f);
    }
}

void EditorLogic::OnUpKeyDown()
{
    auto& scene = g_pSceneManager->GetScene();
    auto pCameraNode = scene.GetFirstCameraNode();
    if (pCameraNode) {
        auto local_axis = pCameraNode->GetLocalAxis(); 
        Vector3Df camera_y_axis;
        memcpy(camera_y_axis.data, local_axis.m[1], sizeof(camera_y_axis));

        // move camera along its local axis y direction
        pCameraNode->MoveBy(camera_y_axis);
    }
}

void EditorLogic::OnDownKeyDown()
{
    auto& scene = g_pSceneManager->GetScene();
    auto pCameraNode = scene.GetFirstCameraNode();
    if (pCameraNode) {
        auto local_axis = pCameraNode->GetLocalAxis(); 
        Vector3Df camera_y_axis;
        memcpy(camera_y_axis.data, local_axis.m[1], sizeof(camera_y_axis));

        // move camera along its local axis -y direction
        pCameraNode->MoveBy(camera_y_axis * -1.0f);
    }
}

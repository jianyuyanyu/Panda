#include <iostream>
#include "BilliardGameLogic.hpp"
#include "GraphicsManager.hpp"
#include "SceneManager.hpp"
#include "IPhysicsManager.hpp"

namespace Panda
{
    int BilliardGameLogic::Initialize()
    {
        int result;

        std::cout << "[BilliardGameLogic] Billiard Game Logic Initialize" << std::endl;
        std::cout << "[BilliardGameLogic] Start Loading Game Scene" << std::endl;
        result = g_pSceneManager->LoadScene("Scene/billiard.ogex");

        return result;
    }

    void BilliardGameLogic::Finalize()
    {
        std::cout << "Billiard Game Logic Finalize" << std::endl;
    }

    void BilliardGameLogic::Tick()
    {

    }

    void BilliardGameLogic::OnLeftKey()
    {
        auto ptr = g_pSceneManager->GetSceneGeometryNode("pbb_cue");
        if (auto node = ptr.lock())
        {
            auto rigicBody = node ->RigidBody();
            if (rigicBody)
            {
				g_pPhysicsManager->ApplyCentralForce(rigicBody, Vector3Df({ -100.0f, 0.0f, 0.0f }));
            }
        }
    }
}
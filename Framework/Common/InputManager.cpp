#include <iostream>
#include "InputManager.hpp"
#include "GraphicsManager.hpp"
#include "PandaMath.hpp"

namespace Panda
{
    int InputManager::Initialize()
    {
        return 0;
    }

    void InputManager::Finalize()
    {

    }

    void InputManager::Tick()
    {

    }

    void InputManager::UpArrowKeyDown()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Up Arrow Key Donw!" << std::endl;
        #endif
        g_pGraphicsManager->WorldRotateX(PI / 60.0f);
    }

    void InputManager::UpArrowKeyUp()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Up Arrow Key Up!" << std::endl;
        #endif
    }

    void InputManager::DownArrowKeyDown()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Down Arrow Key Down!" << std::endl;
        #endif
        g_pGraphicsManager->WorldRotateX(-PI / 60.0f);
    }

    void InputManager::DownArrowKeyUp()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Down Arrow Key Up!" << std::endl;
        #endif
    }

    void InputManager::LeftArrowKeyDown()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Left Arrow Key Down!" << std::endl;
        #endif
        g_pGraphicsManager->WorldRotateY(-PI / 60.0f);
    }

    void InputManager::LeftArrowKeyUp()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Left Arrow Key Up!" << std::endl;
        #endif
    }

    void InputManager::RightArrowKeyDown()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Right Arrow Key Down!" << std::endl;
        #endif
        g_pGraphicsManager->WorldRotateY(PI / 60.0f);
    }

    void InputManager::RightArrowKeyUp()
    {
        #ifdef DEBUG
        std::cerr << "[InputManager] Right Arrow Key Up!" << std::endl;
        #endif
    }
}
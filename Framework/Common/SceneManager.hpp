#pragma once
#include "PandaMath.hpp"
#include "IRuntimeModule.hpp"
#include "SceneNode.hpp"

namespace Panda
{
    class SceneManager : implements IRuntimeModule
    {
        public:
            virtual ~SceneManager();

            virtual int Initialize();
            virtual void Finalize();

            virtual void Tick();

            void LoadOgexScene(const char* sceneFileName);

        protected:
            std::unique_ptr<BaseSceneNode> m_RootNode;
    };

    extern SceneManager* g_pSceneManager;
}

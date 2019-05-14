#pragma once
#include "PandaMath.hpp"
#include "IRuntimeModule.hpp"
#include "SceneParser.hpp"

namespace Panda
{
    class SceneManager : implements IRuntimeModule
    {
        public:
            virtual ~SceneManager();

            virtual int Initialize();
            virtual void Finalize();

            virtual void Tick();

            int LoadScene(const char* sceneFileName);

            const Scene& GetScene();

        protected:
            bool LoadOgexScene(const char* ogexSceneFileName);

        protected:
            std::unique_ptr<Scene> m_pScene;
    };

    extern SceneManager* g_pSceneManager;
}

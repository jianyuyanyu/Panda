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

            bool IsSceneChanged();
            void NotifySceneIsRenderingQueued();
            const Scene& GetScene();
            void ResetScene();

        protected:
            bool LoadOgexScene(const char* ogexSceneFileName);

        protected:
            std::shared_ptr<Scene> m_pScene;
            bool m_DirtyFlag = false;
    };

    extern SceneManager* g_pSceneManager;
}

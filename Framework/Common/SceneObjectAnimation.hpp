#pragma once
#include "BaseSceneObject.hpp"
#include "Animatable.hpp"
#include "SceneObjectTransform.hpp"
#include "Curve.hpp"
#include "SceneObjectTrack.hpp"

namespace Panda
{
    class SceneObjectAnimationClip : public BaseSceneObject, implements Animatable<float>
    {
        private:
            int m_Index = 0;
            bool m_IsLoop = false;
            std::vector<std::shared_ptr<SceneObjectTrack>> m_Tracks;

        public:
            SceneObjectAnimationClip() = delete;
            SceneObjectAnimationClip(int index) : BaseSceneObject(SceneObjectType::kSceneObjectTypeAnimationClip),
                m_Index(index)
            {}
            int GetIndex() {return m_Index;}

            void AddTrack(std::shared_ptr<SceneObjectTrack>& track);
            void Update(const float timePoint) final;

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectAnimationClip& obj);
    };
}
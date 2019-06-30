#include "SceneObjectAnimation.hpp"

namespace Panda
{
    void SceneObjectAnimationClip::AddTrack(std::shared_ptr<SceneObjectTrack>& track)
    {
        m_Tracks.push_back(track);
    }

    void SceneObjectAnimationClip::Update(const float timePoint)
    {
        for (auto track : m_Tracks)
        {
            track->Update(timePoint);
        }
    }
}
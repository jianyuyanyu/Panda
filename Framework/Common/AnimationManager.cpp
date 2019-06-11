#include "AnimationManager.hpp"
#include "SceneManager.hpp"

namespace Panda
{
    int AnimationManager::Initialize()
    {
        const Scene& scene = g_pSceneManager->GetScene();

        for (auto node : scene.AnimatableNodes)
        {
            auto pNode = node.lock();
            if (pNode)
            {
                BaseSceneNode::AnimationClipIterator it;
                if (pNode->GetFirstAnimationClip(it))
                {
                    do
                    {
                        AddAnimationClip(it->second);
                    } while (pNode->GetNextAnimationClip(it));
                }
            }
        }

        return 0;
    }

    void AnimationManager::Finalize()
    {

    }

    void AnimationManager::Tick()
    {
        if (!m_IsTimeLineStarted)
        {
            m_TimeLineStartPoint = m_Clock.now();
            m_IsTimeLineStarted = true;
        }

        m_TimeLineValue = m_Clock.now() - m_TimeLineStartPoint;

        for (auto clip : m_AnimationClips)
        {
            clip->Update(m_TimeLineValue.count());
        }
    }

    void AnimationManager::AddAnimationClip(std::shared_ptr<SceneObjectAnimationClip> clip)
    {
        m_AnimationClips.push_back(clip);
    }
}
#pragma once
#include "BaseSceneNode.hpp"

namespace Panda
{
    class SceneLightNode : public SceneNode<SceneObjectLight>
    {
        protected:
            bool    m_IsShadow;

        public:
            using SceneNode::SceneNode;

            void SetIfCastShadow(bool shadow) {m_IsShadow = shadow;}
            const bool CastShadow() {return m_IsShadow;}
    };
}
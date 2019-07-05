#pragma once
#include "BaseSceneNode.hpp"

namespace Panda
{
    class SceneCameraNode : public SceneNode<SceneObjectCamera>
    {
        protected:
            Vector3Df m_Target;

        public:
            using SceneNode::SceneNode;

            void SetTarget(Vector3Df& target) {m_Target = target;}
            const Vector3Df& GetTarget() {return m_Target;}
            Matrix3f GetLocalAxis()
            {
                Matrix3f result;
                auto pTransform = GetCalculatedTransform();
                Vector3Df target = GetTarget();
                Vector3Df cameraPosition = Vector3Df(0.0f);
                TransformCoord(cameraPosition, *pTransform);
				Vector3Df up({ 0.0f, 0.0f, 1.0f });
                Vector3Df cameraZAxis = cameraPosition - target;
				cameraZAxis = Normalize(cameraZAxis);
                Vector3Df cameraXAxis;
                Vector3Df cameraYAxis;
                cameraXAxis = CrossProduct(cameraZAxis, up);
                cameraYAxis = CrossProduct(cameraXAxis, cameraZAxis);
                result.v[0] = cameraXAxis;
                result.v[1] = cameraYAxis;
                result.v[2] = cameraZAxis;

                return result;
            }
    };
}
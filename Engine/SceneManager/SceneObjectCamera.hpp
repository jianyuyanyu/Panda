#pragma once
#include "BaseSceneObject.hpp"
#include "Math/PandaMath.hpp"

namespace Panda
{
    // camera
    class SceneObjectCamera : public BaseSceneObject
    {
        protected:
            float m_Aspect;
            float m_NearClipDistance;
            float m_FarClipDistance;

        public:
            void SetColor(std::string& attrib, Vector4Df& color)
            {
                // TODO: extension
            }

            void SetParam(std::string& attrib, float param)
            {
                if (attrib == "near")
                {
                    m_NearClipDistance = param;
                }
                else if (attrib == "far")
                {
                    m_FarClipDistance = param;
                }
            }

            void SetTexture(std::string& attrib, std::string& textureName)
            {
                // TODO: extension
            }

            float GetNearClipDistance() const {return m_NearClipDistance;}
            float GetFarClipDistance() const {return m_FarClipDistance;}
        public:
            SceneObjectCamera() : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeCamera), m_Aspect(16.0f / 9.0f), m_NearClipDistance(1.0f), m_FarClipDistance(100.0f) 
                {}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectCamera& obj);
    };

    class SceneObjectOrthogonalCamera : public SceneObjectCamera
    {
        public:
            using SceneObjectCamera::SceneObjectCamera;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectOrthogonalCamera& obj);
    };

    class SceneObjectPerspectiveCamera : public SceneObjectCamera
    {
        protected:
            float m_Fov;

        public:
            void SetParam(std::string& attrib, float param)
            {
                // TODO: handle fovx, fovy
                if (attrib == "fov")
                {
                    m_Fov = param;
                }
                SceneObjectCamera::SetParam(attrib, param);
            }
            SceneObjectPerspectiveCamera(float fov = PI / 2.0f) :
                SceneObjectCamera(), m_Fov(fov)
                {}
            
            float GetFov() const {return m_Fov;}
            friend std::ostream& operator<<(std::ostream& out, const SceneObjectPerspectiveCamera& obj);
    };
}
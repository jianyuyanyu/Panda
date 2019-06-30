#pragma once
#include <functional>
#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"

namespace Panda
{
    typedef std::function<float(float /* Intensity */, float /* Distance */)> AttenFunc;
    
    float DefaultAttenFunc (float intensity, float distance);

    class SceneObjectLight : public BaseSceneObject
    {
        protected:
            Color       m_LightColor;
            float       m_Intensity;
            AttenFunc   m_LightAttenuation;
            bool        m_IsCastShadows;
            std::string m_Texture;

        public:
            void SetIfCastShadow(bool shadow) {m_IsCastShadows = shadow;}

            void SetColor(std::string& attrib, Vector4Df& color)
            {
                if (attrib == "light")
                {
                    m_LightColor = color;
                }
            }

            void SetParam(std::string& attrib, float param)
            {
                if (attrib == "intensity")
                {
                    m_Intensity = param;
                }
            }

            void SetTexture(std::string& attrib, std::string& textureName)
            {
                if (attrib == "projection")
                {
                    m_Texture = textureName;
                }
            }

            void SetAttenuation(AttenFunc func)
            {
                m_LightAttenuation = func;
            }

            const Color& GetColor() {return m_LightColor;}
            float GetIntensity() {return m_Intensity;}

        protected:
            // can only used as base class of delivered lighting objects
            SceneObjectLight() : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeLight), m_LightColor(Vector4Df(1.0f)), m_Intensity(100.0f),
                m_LightAttenuation(DefaultAttenFunc), m_IsCastShadows(false) 
            {}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectLight& obj);
    };

    // point light
    class SceneObjectPointLight : public SceneObjectLight
    {
        public:
            using SceneObjectLight::SceneObjectLight;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectPointLight& obj);
    };

    // spot light
    class SceneObjectSpotLight : public SceneObjectLight
    {
        public:
            float   m_ConeAngle;
            float   m_PenumbraAngle;

        public:
            SceneObjectSpotLight() :
                SceneObjectLight(), m_ConeAngle(PI / 4.0f), m_PenumbraAngle(PI / 3.0f)
            {
            }

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectSpotLight& obj);
    };

	// infinite light
	class SceneObjectInfiniteLight : public SceneObjectLight
	{
	public:
		using SceneObjectLight::SceneObjectLight;

		friend std::ostream& operator<<(std::ostream& out, const SceneObjectInfiniteLight& obj);
	};
}
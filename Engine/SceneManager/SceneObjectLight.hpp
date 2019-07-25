#pragma once
#include <functional>
#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"

namespace Panda
{
	ENUM(AttenCurveType)
	{
		kAttenLinear = 0,
		kAttenSmooth = 1,
		kAttenInverse = 2,
		kAttenInverseSquare = 3
	};

    struct AttenCurve
    {
        AttenCurveType type;
        union AttenCurveParams
        {
            struct LinearParam {float beginAtten; float endAtten;} linearParams;
            struct SmoothParam {float beginAtten; float endAtten;} smoothParams;
            struct InverseParam {float scale; float offset; float kl; float kc;} inverseParams;
            struct InverseSquareParam {float scale; float offset; float kq; float kl; float kc;} inverseSquareParams;
        } u;

        AttenCurve() : type(AttenCurveType::kAttenLinear),
            u({{0.0f, 1.0f}})
            {}
    };

    class SceneObjectLight : public BaseSceneObject
    {
        protected:
            Color       m_LightColor;
            float       m_Intensity;
            AttenCurve  m_DistanceAttenuation;
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

            void SetDistanceAttenuation(AttenCurve curve)
            {
                m_DistanceAttenuation = curve;
            }

            const AttenCurve& GetDistanceAttenuation()
            {
                return m_DistanceAttenuation;
            }

            const Color& GetColor() {return m_LightColor;}
            float GetIntensity() {return m_Intensity;}

        protected:
            // can only used as base class of delivered lighting objects
            SceneObjectLight(const SceneObjectType type) : 
                BaseSceneObject(type), m_LightColor(Vector4Df(1.0f)), m_Intensity(1.0f),
                m_IsCastShadows(false) 
            {}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectLight& obj);
    };

    // point light
    class SceneObjectPointLight : public SceneObjectLight
    {
        public:
            SceneObjectPointLight() : SceneObjectLight(SceneObjectType::kSceneObjectTypeLightPoint) {}

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectPointLight& obj);
    };

    // spot light
    class SceneObjectSpotLight : public SceneObjectLight
    {
        protected:
            float   m_ConeBeginAngle;
            float   m_ConeEndAngle;
            AttenCurve m_LightAngleAttenuation;

        public:
            SceneObjectSpotLight() :
                SceneObjectLight(SceneObjectType::kSceneObjectTypeLightSpot)
            {}

            void SetAngleAttenuation(AttenCurve curve)
            {
                m_LightAngleAttenuation = curve;
            }

            const AttenCurve& GetAngleAttenuation()
            {
                return m_LightAngleAttenuation;
            }

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectSpotLight& obj);
    };

	// infinite light
	class SceneObjectInfiniteLight : public SceneObjectLight
	{
	public:
		SceneObjectInfiniteLight():SceneObjectLight(SceneObjectType::kSceneObjectTypeLightInfinite) {}

		friend std::ostream& operator<<(std::ostream& out, const SceneObjectInfiniteLight& obj);
	};

	// area light
	class SceneObjectAreaLight : public SceneObjectLight
	{
		protected:
			Vector2Df	m_LightDimension;

	public:
		SceneObjectAreaLight() : SceneObjectLight(SceneObjectType::kSceneObjectTypeLightArea),
			m_LightDimension({ 1.0f, 1.0f })
		{}

		const Vector2Df& GetDimension() const
		{
			return m_LightDimension;
		}

		void SetDimension(const Vector2Df& dimension)
		{
			m_LightDimension = dimension;
		}

		void SetDimension(float width, float height)
		{
			m_LightDimension = { width, height };
		}
		
		friend std::ostream& operator<<(std::ostream& out, const SceneObjectAreaLight& obj);
	};
}
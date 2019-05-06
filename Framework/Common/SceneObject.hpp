#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "Guid.hpp"
#include "Image.hpp"
#include "portable.hpp"

namespace Panda
{
    namespace details
    {
        constexpr int32_t i32(const char* s, int32_t v)
        {
            return *s? i32(s + 1, v * 256 + *s) : v;
        }
    }

    constexpr int32_t operator "" _i32(const char* s, size_t) {
        return details::i32(s, 0);
    }

    ENUM(SceneObjectType)
    {
        kSceneObjectTypeMesh    = "MESH"_i32,
        kSceneObjectTypeMaterial= "MATL"_i32,
        kSceneObjectTypeTexture = "TXTU"_i32,
        kSceneObjectTypeLight   = "LGHT"_i32,
        kSceneObjectTypeCamera  = "CAMR"_i32,
        kSceneObjectTypeAnimator= "ANIM"_i32,
        kSceneObjectTypeClip    = "CLIP"_i32,
        kSceneObjectTypeVertexArray = "VARR"_i32,
        kSceneObjectTypeIndexArray = "IARR"_i32,
        kSceneObjectTypeGeometry = "GEOM"_i32,
    };

    std::ostream& operator<<(std::ostream& out, SceneObjectType type)
    {
        int32_t n = static_cast<int32_t> (type);
        n = endian_net<int32_t>(n);
        char* c = reinterpret_cast<char*>(&n);

        for (size_t i = 0; i < sizeof (int32_t); ++i)
        {
            out << *c++;
        }

        return out;
    }

    using namespace xg;

    class BaseSceneObject
    {
    protected:
        Guid m_Guid;
        SceneObjectType m_Type;
    protected:
        // can only be used as base class
        BaseSceneObject(SceneObjectType type) : m_Type(type) { m_Guid = newGuid(); }
        BaseSceneObject(Guid& guid, SceneObjectType type): m_Guid(guid), m_Type(type) {}
        BaseSceneObject(Guid&& guid, SceneObjectType type) : m_Guid(std::move(guid)), m_Type(type) {}
        BaseSceneObject(BaseSceneObject&& obj): m_Guid(std::move(obj.m_Guid)), m_Type(obj.m_Type) {}
        BaseSceneObject& operator=(BaseSceneObject&& obj){this ->m_Guid = std::move(obj.m_Guid); this->m_Type = obj.m_Type; obj.m_Guid = 0; return *this;}

    private:
        // a type must be specified
        BaseSceneObject() = delete;
        // can not be copied
        BaseSceneObject(BaseSceneObject& obj) = delete;
        BaseSceneObject& operator=(BaseSceneObject& obj) = delete;

    public:
        const Guid& GetGuid() const {return m_Guid;}
        const SceneObjectType GetType() const {return m_Type;}
    
        friend std::ostream& operator<<(std::ostream& out, const BaseSceneObject& obj)
        {
            out << "BaseSceneObj" << std::endl;
            out << "------------" << std::endl;
            out << "GUID: " << obj.m_Guid << std::endl;
            out << "Type: " << obj.m_Type << std::endl;

            return out;
        }
    };

    ENUM(VertexDataType)
    {
        kVertexDataTypeFloat1   = "FLT1"_i32,
        kVertexDataTypeFloat2   = "FLT2"_i32,
        kVertexDataTypeFloat3   = "FLT3"_i32,
        kVertexDataTypeFloat4   = "FLT4"_i32,
        kVertexDataTypeDouble1   = "DUB1"_i32,
        kVertexDataTypeDouble2   = "DUB2"_i32,
        kVertexDataTypeDouble3   = "DUB3"_i32,
        kVertexDataTypeDouble4   = "DUB4"_i32,
    };

    class SceneObjectVertexArray : public BaseSceneObject
    {
    protected:
        std::string m_Attribute;
        uint32_t m_MorphTargetIndex;
        VertexDataType m_DataType;

        void* m_pDataFloat;

        size_t m_DataSize;
        
        public:
            SceneObjectVertexArray(const char* attr, void* data, size_t data_size, VertexDataType data_type, uint32_t morph_index = 0)
                :BaseSceneObject(SceneObjectType::kSceneObjectTypeVertexArray), m_Attribute(attr), 
                m_MorphTargetIndex(morph_index), m_DataType(data_type), m_pDataFloat(data), 
                m_DataSize(data_size) {}
    };

    ENUM (IndexDataType)
    {
        kIndexDataTypeInt16 = "_I16"_i32,
        kIndexDataTypeInt32 = "_I32"_i32
    };

    class SceneObjectIndexArray : public BaseSceneObject
    {
    protected:
        uint32_t        m_MaterialIndex;
        size_t          m_RestartIndex;
        IndexDataType   m_DataType;

        void*           m_pData;
        size_t          m_DataSize;

        public:
            SceneObjectIndexArray(uint32_t material_index, IndexDataType data_type = IndexDataType::kIndexDataTypeInt16, uint32_t restart_index = 0) :
                BaseSceneObject(SceneObjectType::kSceneObjectTypeIndexArray), m_MaterialIndex(material_index), m_RestartIndex(restart_index),
                m_DataType(data_type) {}
    };

    class SceneObjectMesh : public BaseSceneObject
    {
    protected:
        std::vector<SceneObjectIndexArray>  m_IndexArray;
        std::vector<SceneObjectVertexArray> m_VertexArray;

        bool    m_IsVisible;
        bool    m_IsShadow;
        bool    m_IsMotionBlur;

    public:
        SceneObjectMesh(bool visible = true, bool shadow = true, bool motion_blur = true) : 
            BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh), m_IsVisible(visible), m_IsShadow(shadow), m_IsMotionBlur(motion_blur)
        {}
        void AddIndexArray(SceneObjectIndexArray&& array) { m_IndexArray.push_back(std::move(array));}
        void AddVertexArray(SceneObjectVertexArray&& array) {m_VertexArray.push_back(std::move(array));}

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMesh& obj)
        {
            out << static_cast<const BaseSceneObject&>(obj) << std::endl;
            out << "Visible: " << obj.m_IsVisible << std::endl;
            out << "Shadow: " << obj.m_IsVisible << std::endl;
            out << "Motion Blur: " << obj.m_IsMotionBlur << std::endl;

            return out;
        }
    };

    template <typename T>
    struct ParameterMap
    {
        bool IsUsingSingleValue;

        union 
        {
            T Value;
            std::shared_ptr<Image> Map;
        };

        ParameterMap(T value) : IsUsingSingleValue(true), Value(value) {}
        ParameterMap(const ParameterMap& rhs)
        {
            IsUsingSingleValue = rhs.IsUsingSingleValue;

            if (IsUsingSingleValue)
                Value = rhs.Value;
            else
                Map = rhs.Map;
        }

        ParameterMap(ParameterMap&& rhs)
        {
            IsUsingSingleValue = rhs.IsUsingSingleValue;

            if (IsUsingSingleValue)
                Value = rhs.Value;
            else
            {
                Map = std::move(rhs.Map);
                rhs.Map.reset();
            }
        }

        ~ParameterMap()
        {
            if (!IsUsingSingleValue)
                Map.reset();
        }

        friend std::ostream& operator<<(std::ostream& out, const ParameterMap&obj)
        {
            if (obj.IsUsingSingleValue)
            {
                out << "Parameter Type: Single Value" << std::endl;
                //out << "Parameter Value: " << obj.Value << std::endl;
            }
            else
            {
                out << "Parameter Type: Map" << std::endl;
            }

            return out;
        }
    };

    typedef ParameterMap<ColorRGBAf>  Color;
    typedef ParameterMap<Vector3D> Normal;
    typedef ParameterMap<float> Parameter;

    // PBR material
    class SceneObjectMaterial : public BaseSceneObject
    {
        protected:
            Color               m_BaseColor;
            Parameter           m_Metallic;
            Parameter           m_Roughness;
            Normal              m_Normal;
            Parameter           m_Specular;
            Parameter           m_AmbientOcclusion;

        public:
            SceneObjectMaterial(Color&& base_color = ColorRGBAf(1.0f), Parameter&& metallic = 0.0f, Parameter&& roughness = 0.0f,
                Normal&& normal = Vector3D(0.0f, 0.0f, 1.0f), Parameter&& specular = 0.0f, Parameter&& ao = 0.0f) : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_BaseColor(std::move(base_color)), m_Metallic(std::move(metallic)),
                m_Roughness(std::move(roughness)), m_Normal(std::move(normal)), m_Specular(std::move(specular)), m_AmbientOcclusion(std::move(ao))
                {}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectMaterial& obj)
            {
                out << static_cast<const BaseSceneObject&>(obj) << std::endl;
                out << "Albedo: " << obj.m_BaseColor << std::endl;
                out << "Metallic: " << obj.m_BaseColor << std::endl;
                out << "Roughness: " << obj.m_Roughness << std::endl;
                out << "Normal: " << obj.m_Normal << std::endl;
                out << "Specular: " << obj.m_Specular << std::endl;
                out << "Ambient Occlusion: " << obj.m_AmbientOcclusion << std::endl;

                return out;
            }
    };

    class SceneObjectGeometry : public BaseSceneObject
    {
        protected:
            std::vector<SceneObjectMesh> m_Mesh;

        public:
            void AddMesh(SceneObjectMesh&& mesh) {m_Mesh.push_back(std::move(mesh));}
            SceneObjectGeometry() : BaseSceneObject(SceneObjectType::kSceneObjectTypeGeometry) {}
    };

    typedef float (*AttenFunc) (float /* Intensity */, float /* Distance */);

    float DefaultAttenFunc (float intensity, float distance)
    {
        return intensity / (1 + distance);
    }

    class SceneObjectLight : public BaseSceneObject
    {
        protected:
            Color       m_LightColor;
            float       m_Intensity;
            AttenFunc   m_LightAttenuation;
            float       m_NearClipDistance;
            float       m_FarClipDistance;
            bool        m_IsCastShadows;

        protected:
            // can only used as base class of delivered lighting objects
            SceneObjectLight(Color&& color = ColorRGBAf(1.0f), float intensity = 10.0f, AttenFunc atten_fun = DefaultAttenFunc, 
                float near_clip = 1.0f, float far_clip = 100.0f, bool cast_shadows = false) : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeLight), m_LightColor(std::move(color)), m_Intensity(intensity),
                m_LightAttenuation(atten_fun), m_NearClipDistance(near_clip), m_FarClipDistance(far_clip), m_IsCastShadows(cast_shadows) 
            {}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectLight& obj)
            {
                out << static_cast<const BaseSceneObject&>(obj) << std::endl;
                out << "Color: " << obj.m_LightColor << std::endl;
                out << "Intensity: " << obj.m_Intensity << std::endl;
                out << "Near Clip Distance: " << obj.m_NearClipDistance << std::endl;
                out << "Far Clip Distance: " << obj.m_FarClipDistance << std::endl;
                out << "Cast Shadows: " << obj.m_IsCastShadows << std::endl;

                return out;
            }
    };

    // point light
    class SceneObjectPointLight : public SceneObjectLight
    {
        public:
            using SceneObjectLight::SceneObjectLight;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectPointLight& obj)
        {
            out << static_cast<const SceneObjectLight&>(obj) << std::endl;
            out << "Light Type: Point" << std::endl;

            return out;
        }
    };

    // spot light
    class SceneObjectSpotLight : public SceneObjectLight
    {
        public:
            float   m_ConeAngle;
            float   m_PenumbraAngle;

        public:
            SceneObjectSpotLight(Color&& color = ColorRGBAf(1.0f), float intensity = 10.0f, AttenFunc atten_func = DefaultAttenFunc, float near_clip = 1.0f,
                float far_clip = 100.0f, bool cast_shadows = false, float cone_angle = PI / 4.0f, float penumbra_angle = PI / 3.0f) :
                SceneObjectLight(std::move(color), intensity, atten_func, near_clip, far_clip, cast_shadows), m_ConeAngle(cone_angle), m_PenumbraAngle(penumbra_angle)
            {
            }

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectSpotLight& obj)
            {
                out << static_cast<const SceneObjectLight&>(obj) << std::endl;
                out << "Light Type: Spot" << std::endl;
                out << "Cone Angle: " << obj.m_ConeAngle << std::endl;
                out << "Penumbra Angle: " << obj.m_PenumbraAngle << std::endl;
                return out;
            }
    };

    // camera
    class SceneObjectCamera : public BaseSceneObject
    {
        protected:
            float m_Aspect;
            float m_NearClipDistance;
            float m_FarClipDistance;
        public:
            SceneObjectCamera(float aspect = 16.0f / 9.0f, float near_clip = 1.0f, float far_clip = 100.0f) : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeCamera), m_Aspect(aspect), m_NearClipDistance(near_clip), m_FarClipDistance(far_clip) 
                {}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectCamera& obj)
            {
                out << static_cast<const BaseSceneObject&>(obj) << std::endl;
                out << "Aspect: " << obj.m_Aspect << std::endl;
                out << "Near Clip Distance: " << obj.m_NearClipDistance << std::endl;
                out << "Far Clip Disance: " << obj.m_FarClipDistance << std::endl;

                return out;
            }
    };

    class SceneObjectOrthogonalCamera : public SceneObjectCamera
    {
        public:
            using SceneObjectCamera::SceneObjectCamera;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectOrthogonalCamera& obj)
        {
            out << static_cast<const SceneObjectCamera&>(obj) << std::endl;
            out << "Camera Type: Orthogonal" << std::endl;
            return out;
        }
    };

    class SceneObjectPerspectiveCamera : public SceneObjectCamera
    {
        protected:
            float m_Fov;

        public:
            SceneObjectPerspectiveCamera(float aspect = 16.0f / 9.0f, float near_clip = 1.0f, float far_clip = 100.0f, float fov = PI / 2.0) :
                SceneObjectCamera(aspect, near_clip, far_clip), m_Fov(fov)
                {}
            
            friend std::ostream& operator<<(std::ostream& out, const SceneObjectPerspectiveCamera& obj)
            {
                out << static_cast<const SceneObjectCamera&>(obj) << std::endl;
                out << "Camera Type: Perspective" << std::endl;
                out << "FOV: " << obj.m_Fov << std::endl;

                return out;
            }
    };
}

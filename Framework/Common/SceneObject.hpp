#pragma once
#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "Guid.hpp"
#include "Image.hpp"
#include "portable.hpp"
#include "PandaMath.hpp"

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
        virtual ~BaseSceneObject() {}

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
            out << "SceneObject" << std::endl;
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

    std::ostream& operator<<(std::ostream& out, VertexDataType type)
    {
        int32_t n = static_cast<int32_t>(type);
        n = endian_net<int32_t> (n);
        char* c = reinterpret_cast<char*>(&n);

        for (size_t i = 0; i < sizeof(int32_t); ++i)
        {
            out << *c++;
        }

        return out;
    }

    class SceneObjectVertexArray
    {
    protected:
        const std::string       m_Attribute;
        const uint32_t          m_MorphTargetIndex;
        const VertexDataType    m_DataType;

        const void*             m_pData;

        const size_t            m_DataSize;
        
        public:
            SceneObjectVertexArray(const char* attr = "", const uint32_t morphIndex = 0, const VertexDataType dataType = VertexDataType::kVertexDataTypeFloat3,
                const void* data = nullptr, const size_t dataSize = 0)
                : m_Attribute(attr), m_MorphTargetIndex(morphIndex), m_DataType(dataType), m_pData(data), m_DataSize(dataSize)
                {}
            SceneObjectVertexArray(SceneObjectVertexArray& arr) = default; // this two might be modified
            SceneObjectVertexArray(SceneObjectVertexArray&& arr) = default;

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectVertexArray& obj)
            {
                out << "Attribute: " << obj.m_Attribute << std::endl;
                out << "Morph Target Index: 0x" << obj.m_MorphTargetIndex << std::endl;
                out << "Data Type: " << obj.m_DataType << std::endl;
                out << "Data Size: 0x" << obj.m_DataSize << std::endl;
                out << "Data: ";
                for (size_t i = 0; i < obj.m_DataSize; ++i)
                {
                    out << *(reinterpret_cast<const float*>(obj.m_pData) + i) << ' ';
                }

                return out;
            }
    };

    ENUM (IndexDataType)
    {
        kIndexDataTypeInt8  = "I8  "_i32,
        kIndexDataTypeInt16 = "I16 "_i32,
        kIndexDataTypeInt32 = "I32 "_i32,
        kIndexDataTypeInt64 = "I64 "_i32,
    };

    std::ostream& operator<<(std::ostream& out, IndexDataType type)
    {
        int32_t n = static_cast<int32_t>(type);
        n = endian_net<int32_t>(n);
        char* c = reinterpret_cast<char*>(&n);

        for (size_t i = 0; i < sizeof(int32_t); ++i)
        {
            out << *c++;
        }
        return out;
    }

    class SceneObjectIndexArray
    {
        protected:
            const uint32_t        m_MaterialIndex;
            const size_t          m_RestartIndex;
            const IndexDataType   m_DataType;

            const void*           m_pData;
            const size_t          m_DataSize;

            public:
                SceneObjectIndexArray(const uint32_t material_index = 0, const uint32_t restart_index = 0, const IndexDataType data_type = IndexDataType::kIndexDataTypeInt16, 
                    const void* data = nullptr, const size_t dataSize = 0) :
                    m_MaterialIndex(material_index), m_RestartIndex(restart_index), m_DataType(data_type),
                    m_pData(data), m_DataSize(dataSize)
                    {}
                SceneObjectIndexArray(SceneObjectIndexArray& arr) = default;
                SceneObjectIndexArray(SceneObjectIndexArray&& arr) = default;

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectIndexArray& obj)
            {
                out << "Material Index: 0x" << obj.m_MaterialIndex << std::endl;
                out << "Restart Index: 0x" << obj.m_RestartIndex << std::endl;
                out << "Data Type: " << obj.m_DataType << std::endl;
                out << "Data Size: 0x" << obj.m_DataSize << std::endl;
                out << "Data: ";

                for (size_t i = 0; i < obj.m_DataSize; ++i)
                {
                    switch(obj.m_DataType)
                    {
                        case IndexDataType::kIndexDataTypeInt8:
                            out << "0x" << *(reinterpret_cast<const uint8_t*>(obj.m_pData) + i) << ' ';
                            break;
                        case IndexDataType::kIndexDataTypeInt16:
                            out << "0x" << *(reinterpret_cast<const uint16_t*>(obj.m_pData) + i) << ' ';
                            break;
                        case IndexDataType::kIndexDataTypeInt32:
                            out << "0x" << *(reinterpret_cast<const uint32_t*>(obj.m_pData) + i) << ' ';
                            break;
                        case IndexDataType::kIndexDataTypeInt64:
                            out << "0x" << *(reinterpret_cast<const uint64_t*>(obj.m_pData) + i) << ' ';
                            break;
                        default:
                            break;
                    }
                }

                return out;
            }
                    
    };

    ENUM(PrimitiveType) {
		kPrimitiveTypeNone = "NONE"_i32,        ///< No particular primitive type.
		kPrimitiveTypePointList = "PLST"_i32,   ///< For N>=0, vertex N renders a point.
		kPrimitiveTypeLineList = "LLST"_i32,    ///< For N>=0, vertices [N*2+0, N*2+1] render a line.
		kPrimitiveTypeLineStrip = "LSTR"_i32,   ///< For N>=0, vertices [N, N+1] render a line.
		kPrimitiveTypeTriList = "TLST"_i32,     ///< For N>=0, vertices [N*3+0, N*3+1, N*3+2] render a triangle.
		kPrimitiveTypeTriFan = "TFAN"_i32,      ///< For N>=0, vertices [0, (N+1)%M, (N+2)%M] render a triangle, where M is the vertex count.
		kPrimitiveTypeTriStrip = "TSTR"_i32,    ///< For N>=0, vertices [N*2+0, N*2+1, N*2+2] and [N*2+2, N*2+1, N*2+3] render triangles.
		kPrimitiveTypePatch = "PACH"_i32,       ///< Used for tessellation.
		kPrimitiveTypeLineListAdjacency = "LLSA"_i32,       ///< For N>=0, vertices [N*4..N*4+3] render a line from [1, 2]. Lines [0, 1] and [2, 3] are adjacent to the rendered line.
		kPrimitiveTypeLineStripAdjacency = "LSTA"_i32,      ///< For N>=0, vertices [N+1, N+2] render a line. Lines [N, N+1] and [N+2, N+3] are adjacent to the rendered line.
		kPrimitiveTypeTriListAdjacency = "TLSA"_i32,        ///< For N>=0, vertices [N*6..N*6+5] render a triangle from [0, 2, 4]. Triangles [0, 1, 2] [4, 2, 3] and [5, 0, 4] are adjacent to the rendered triangle.
		kPrimitiveTypeTriStripAdjacency = "TSTA"_i32,       ///< For N>=0, vertices [N*4..N*4+6] render a triangle from [0, 2, 4] and [4, 2, 6]. Odd vertices Nodd form adjacent triangles with indices min(Nodd+1,Nlast) and max(Nodd-3,Nfirst).
		kPrimitiveTypeRectList = "RLST"_i32,    ///< For N>=0, vertices [N*3+0, N*3+1, N*3+2] render a screen-aligned rectangle. 0 is upper-left, 1 is upper-right, and 2 is the lower-left corner.
		kPrimitiveTypeLineLoop = "LLOP"_i32,    ///< Like <c>kPrimitiveTypeLineStrip</c>, but the first and last vertices also render a line.
		kPrimitiveTypeQuadList = "QLST"_i32,    ///< For N>=0, vertices [N*4+0, N*4+1, N*4+2] and [N*4+0, N*4+2, N*4+3] render triangles.
		kPrimitiveTypeQuadStrip = "QSTR"_i32,   ///< For N>=0, vertices [N*2+0, N*2+1, N*2+3] and [N*2+0, N*2+3, N*2+2] render triangles.
		kPrimitiveTypePolygon = "POLY"_i32,     ///< For N>=0, vertices [0, N+1, N+2] render a triangle.
	};

    std::ostream& operator<<(std::ostream&out, PrimitiveType type)
    {
        int32_t n = static_cast<int32_t>(type);
        n = endian_net<int32_t>(n);
        char* c = reinterpret_cast<char*>(&n);

        for(size_t i = 0; i < sizeof(int32_t); ++i)
        {
            out << *c++;
        }

        return out;
    }

    class SceneObjectMesh : public BaseSceneObject
    {
    protected:
        std::vector<SceneObjectIndexArray>  m_IndexArray;
        std::vector<SceneObjectVertexArray> m_VertexArray;
        PrimitiveType m_PrimitiveType;

        bool    m_IsVisible;
        bool    m_IsShadow;
        bool    m_IsMotionBlur;

    public:
        SceneObjectMesh(bool visible = true, bool shadow = true, bool motion_blur = true) : 
            BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh), m_IsVisible(visible), m_IsShadow(shadow), m_IsMotionBlur(motion_blur)
        {}
        SceneObjectMesh(SceneObjectMesh&& mesh) :
            BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh),
            m_IndexArray(std::move(mesh.m_IndexArray)),
            m_VertexArray(std::move(mesh.m_VertexArray)),
            m_PrimitiveType(mesh.m_PrimitiveType),
            m_IsVisible(mesh.m_IsVisible),
            m_IsShadow(mesh.m_IsShadow),
            m_IsMotionBlur(mesh.m_IsMotionBlur)
            {}
        void AddIndexArray(SceneObjectIndexArray&& array) { m_IndexArray.push_back(std::move(array));}
        void AddVertexArray(SceneObjectVertexArray&& array) {m_VertexArray.push_back(std::move(array));}
        void SetPrimitiveType(PrimitiveType type) {m_PrimitiveType = type;}
        
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMesh& obj)
        {
            out << static_cast<const BaseSceneObject&>(obj) << std::endl;
            out << "Primitive Type: " << obj.m_PrimitiveType << std::endl;
            out << "This mesh contains 0x" << obj.m_VertexArray.size() << " vertex properties." << std::endl;
            for (size_t i = 0; i < obj.m_VertexArray.size(); ++i)
            {
                out << obj.m_VertexArray[i] << std::endl;
            }
            out << "This mesh contains 0x" << obj.m_IndexArray.size() << " index arrays." << std::endl;
            for(size_t i = 0; i < obj.m_IndexArray.size(); ++i)
            {
                out << obj.m_IndexArray[i] << std::endl;
            }
            out << "Visible: " << obj.m_IsVisible << std::endl;
            out << "Shadow: " << obj.m_IsVisible << std::endl;
            out << "Motion Blur: " << obj.m_IsMotionBlur << std::endl;

            return out;
        }
    };

    // This might be modified in the furture.
    class SceneObjectTexture: public BaseSceneObject
    {
        protected:
            uint32_t m_TexCoordIndex;
            std::string m_Name;

            std::shared_ptr<Image> m_pImage;

            std::vector<Matrix4f> m_Transforms;

        public:
            SceneObjectTexture() : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(0) {}
            SceneObjectTexture(std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(0), m_Name(name) {}
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(coord_index), m_pImage(image) {}
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>&& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(coord_index), m_pImage(std::move(image)) {}
            SceneObjectTexture(SceneObjectTexture&) = default;
            SceneObjectTexture(SceneObjectTexture&&) = default;
            void SetName(std::string& name) {m_Name = name;}
            void AddTransform(Matrix4f& matrix) {m_Transforms.push_back(matrix);}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectTexture& obj)
            {
                out << static_cast<const BaseSceneObject&>(obj) << std::endl;
                out << "Coord Index: " << obj.m_TexCoordIndex << std::endl;
                out << "Name: " << obj.m_TexCoordIndex << std::endl;
                if (obj.m_pImage)
                    out << "Image: " << *obj.m_pImage << std::endl;
                
                return out;
            }
    };

    template <typename T>
    struct ParameterValueMap
    {
        T Value;
        std::shared_ptr<SceneObjectTexture> ValueMap;

		ParameterValueMap() = default;
        ParameterValueMap(const T value) : Value(value) {}
        ParameterValueMap(const std::shared_ptr<SceneObjectTexture>& value) : ValueMap(value) {}
        
        ParameterValueMap(const ParameterValueMap& rhs) = default;
        ParameterValueMap(ParameterValueMap&& rhs) = default;
        
        ParameterValueMap& operator=(const ParameterValueMap& rhs) = default;
        ParameterValueMap& operator=(ParameterValueMap&& rhs) = default;
        ParameterValueMap& operator=(const std::shared_ptr<SceneObjectTexture>& rhs)
        {
            ValueMap = rhs;
            return *this;
        }

        ~ParameterValueMap() = default;

        friend std::ostream& operator<<(std::ostream& out, const ParameterValueMap&obj)
        {
            //out << "Parameter Value: " << obj.Value << std::endl;
            if (obj.ValueMap)
            {
                out << "Parameter Map: " << *obj.ValueMap << std::endl;
            }

            return out;
        }
    };

    typedef ParameterValueMap<ColorRGBAf>  Color;
    typedef ParameterValueMap<Vector3Df> Normal;
    typedef ParameterValueMap<float> Parameter;

    // PBR material
    class SceneObjectMaterial : public BaseSceneObject
    {
        protected:
            std::string         m_Name;
            Color               m_BaseColor;
            Parameter           m_Metallic;
            Parameter           m_Roughness;
            Normal              m_Normal;
            Parameter           m_Specular;
            Parameter           m_AmbientOcclusion;

        public:
            SceneObjectMaterial(const std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(name) {}
            SceneObjectMaterial(std::string&& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(std::move(name)) {}
            SceneObjectMaterial(const std::string& name = "", Color&& base_color = ColorRGBAf(1.0f), Parameter&& metallic = 0.0f, Parameter&& roughness = 0.0f,
                Normal&& normal = Vector3Df(0.0f, 0.0f, 1.0f), Parameter&& specular = 0.0f, Parameter&& ao = 0.0f) : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(name), m_BaseColor(std::move(base_color)), m_Metallic(std::move(metallic)),
                m_Roughness(std::move(roughness)), m_Normal(std::move(normal)), m_Specular(std::move(specular)), m_AmbientOcclusion(std::move(ao))
                {}

            void SetName(const std::string& name) {m_Name = name;}
            void SetName(std::string&& name) {m_Name = std::move(name);}
            void SetColor(std::string& attrib, ColorRGBAf& color)
            {
                if (attrib == "diffuse")
                {
                    m_BaseColor = Color(color);
                }
            }
            void SetParam(std::string& attrib, float param)
            {

            }

            void SetTexture(std::string& attrib, std::string& textureName)
            {
                // if (attrib == "diffuse")
                // {
                //     m_BaseColor = std::make_shared<SceneObjectTexture>(textureName);
                // }
            }

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectMaterial& obj)
            {
                out << static_cast<const BaseSceneObject&>(obj) << std::endl;
                out << "Name: " << obj.m_Name << std::endl;
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
            std::vector<std::shared_ptr<SceneObjectMesh>> m_Mesh;
            bool        m_IsVisible;
            bool        m_IsShadow;
            bool        m_IsMotionBlur;

        public:
            SceneObjectGeometry() : BaseSceneObject(SceneObjectType::kSceneObjectTypeGeometry) {}

            void SetVisibility(bool visible) {m_IsVisible = visible;}
            const bool Visible() {return m_IsVisible;}
            void SetIfCastShadow(bool shadow) {m_IsShadow = shadow;}
            const bool CastShadow() {return m_IsShadow;}
            void SetIfMotionBlur(bool motionBlur) {m_IsMotionBlur = motionBlur;}
            const bool MotionBlur() {return m_IsMotionBlur;}

            void AddMesh(std::shared_ptr<SceneObjectMesh>& mesh) {m_Mesh.push_back(std::move(mesh));}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectGeometry& obj)
            {
                auto count = obj.m_Mesh.size();
                for (decltype(count) i = 0; i < count; ++i)
                {
                    out << "Mesh: " << *obj.m_Mesh[i] << std::endl;
                }

                return out;
            }
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

    class SceneObjectTransform
    {
        protected:
            Matrix4f m_Matrix;
            bool    m_IsSceneObjectOnly;

        public:
            SceneObjectTransform() {m_Matrix.SetIdentity(); m_IsSceneObjectOnly = false;}
            SceneObjectTransform(const Matrix4f& matrix, const bool objectOnly = false) {m_Matrix = matrix; m_IsSceneObjectOnly = objectOnly;}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectTransform& obj)
            {
                out << "Transform Matrix: " << obj.m_Matrix << std::endl;
                out << "Is Object Local: " << obj.m_IsSceneObjectOnly << std::endl;

                return out;
            }
    };

    class SceneObjectTranslation : public SceneObjectTransform
    {
        public:
            SceneObjectTranslation(const char axis, const float amount)
            {
                switch(axis)
                {
                    case 'x':
                        MatrixTranslation(m_Matrix, amount, 0.f, 0.f);
                        break;
                    case 'y':
                        MatrixTranslation(m_Matrix, 0.f, amount, 0.f);
                        break;
                    case 'z':
                        MatrixTranslation(m_Matrix, 0.f, 0.f, amount);
                        break;
                    default:
                        assert(false);
                        break;
                }
            }
            SceneObjectTranslation(const float x, const float y, const float z)
            {
                MatrixTranslation(m_Matrix, x, y, z);
            }
    };

    class SceneObjectRotation : public SceneObjectTransform
    {
        public:
            SceneObjectRotation(const char axis, const float theta)
            {
                switch (axis)
                {
                    case 'x':
                        MatrixRotationX(m_Matrix, theta);
                        break;
                    case 'y':
                        MatrixRotationY(m_Matrix, theta);
                        break;
                    case 'z':
                        MatrixRotationZ(m_Matrix, theta);
                        break;
                    default:
                        assert(false);
                        break;
                }
            }

            SceneObjectRotation(Vector3Df& axis, const float theta)
            {
				axis.Normalize();
                MatrixRotationAxis(m_Matrix, axis, theta);
            }

            SceneObjectRotation(const Quaternion& q)
            {
                MatrixRotationQuaternion(m_Matrix, q);
            }
    };

    class SceneObjectScale : public SceneObjectTransform
    {
        SceneObjectScale(const char axis, const float amount)
        {
            switch(axis)
            {
                case 'x':
                    MatrixScale(m_Matrix, amount, 0.f, 0.f);
                    break;
                case 'y':
                    MatrixScale(m_Matrix, 0.f, amount, 0.f);
                    break;
                case 'z':
                    MatrixScale(m_Matrix, 0.f, 0.f, amount);
                    break;
                default:
                    assert(0);
            }
        }

        SceneObjectScale(const float x, const float y, const float z)
        {
            MatrixScale(m_Matrix, x, y, z);
        }
    };
}

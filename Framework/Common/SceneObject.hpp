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

    std::ostream& operator<<(std::ostream& out, SceneObjectType type);

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
    
        friend std::ostream& operator<<(std::ostream& out, const BaseSceneObject& obj);
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

    std::ostream& operator<<(std::ostream& out, VertexDataType type);

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

            const std::string& GetAttributeName() const {return m_Attribute;}
            VertexDataType GetDataType() const {return m_DataType;}
            size_t GetDataSize() const
            {
                size_t size = m_DataSize;

                switch(m_DataType)
                {
                    case VertexDataType::kVertexDataTypeFloat1:
                    case VertexDataType::kVertexDataTypeFloat2:
                    case VertexDataType::kVertexDataTypeFloat3:
                    case VertexDataType::kVertexDataTypeFloat4:
                        size *= sizeof(float);
                        break;

                    case VertexDataType::kVertexDataTypeDouble1:
                    case VertexDataType::kVertexDataTypeDouble2:
                    case VertexDataType::kVertexDataTypeDouble3:
                    case VertexDataType::kVertexDataTypeDouble4:
                        size *= sizeof(double);
                        break;
                    default:
                        size = 0;
                        assert(0);
                        break;
                }

                return size;
            }

            const void* GetData() const {return m_pData;}

            size_t GetVertexCount() const
            {
                size_t size = m_DataSize;

                switch(m_DataType)
                {
                    case VertexDataType::kVertexDataTypeFloat1:
                        size /= 1;
                        break;
                    case VertexDataType::kVertexDataTypeFloat2:
                        size /= 2;
                        break;
                    case VertexDataType::kVertexDataTypeFloat3:
                        size /= 3;
                        break;
                    case VertexDataType::kVertexDataTypeFloat4:
                        size /= 4;
                        break;
                    case VertexDataType::kVertexDataTypeDouble1:
                        size /= 1;
                        break;
                    case VertexDataType::kVertexDataTypeDouble2:
                        size /= 2;
                        break;
                    case VertexDataType::kVertexDataTypeDouble3:
                        size /= 3;
                        break;
                    case VertexDataType::kVertexDataTypeDouble4:
                        size /= 4;
                        break;
                    default:
                        size = 0;
                        assert(0);
                        break;
                }

                return size;
            }
            friend std::ostream& operator<<(std::ostream& out, const SceneObjectVertexArray& obj);
    };

    ENUM (IndexDataType)
    {
        kIndexDataTypeInt8  = "I8  "_i32,
        kIndexDataTypeInt16 = "I16 "_i32,
        kIndexDataTypeInt32 = "I32 "_i32,
        kIndexDataTypeInt64 = "I64 "_i32,
    };

    std::ostream& operator<<(std::ostream& out, IndexDataType type);

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

                const IndexDataType GetIndexType() const {return m_DataType;}
                const void* GetData() const {return m_pData;}
                size_t GetDataSize() const
                {
                    size_t size = m_DataSize;

                    switch(m_DataType)
                    {
                        case IndexDataType::kIndexDataTypeInt8:
                            size *= sizeof(int8_t);
                            break;
                        case IndexDataType::kIndexDataTypeInt16:
                            size *= sizeof(int16_t);
                            break;
                        case IndexDataType::kIndexDataTypeInt32:
                            size *= sizeof(int32_t);
                            break;
                        case IndexDataType::kIndexDataTypeInt64:
                            size *= sizeof(int64_t);
                            break;
                        default:
                            size = 0;
                            assert(0);
                            break;
                    }

                    return size;
                }

                size_t GetIndexCount() const
                {
                    return m_DataSize;
                }
                friend std::ostream& operator<<(std::ostream& out, const SceneObjectIndexArray& obj);
                    
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

    std::ostream& operator<<(std::ostream&out, PrimitiveType type);

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
        
        size_t GetIndexCount() const {return m_IndexArray.empty()? 0 : m_IndexArray[0].GetIndexCount();}
        size_t GetVertexCount() const {return m_VertexArray.empty() ? 0 : m_VertexArray[0].GetVertexCount();}
        size_t GetVertexPropertiesCount() const {return m_VertexArray.size();}
        const SceneObjectVertexArray& GetVertexPropertyArray(const size_t index) const {return m_VertexArray[index];}
        const SceneObjectIndexArray& GetIndexArray(const size_t index) const {return m_IndexArray[index];}
        const PrimitiveType& GetPrimitiveType() {return m_PrimitiveType;}

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMesh& obj);
    };

    
    class SceneObjectTexture: public BaseSceneObject
    {
        protected:
            std::string m_Name;
            uint32_t m_TexCoordIndex;
            std::shared_ptr<Image> m_pImage;

            std::vector<Matrix4f> m_Transforms;

        public:
            SceneObjectTexture() : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(0) {}
            SceneObjectTexture(std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_Name(name), m_TexCoordIndex(0) {}
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(coord_index), m_pImage(image) {}
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>&& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(coord_index), m_pImage(std::move(image)) {}
            SceneObjectTexture(SceneObjectTexture&) = default;
            SceneObjectTexture(SceneObjectTexture&&) = default;
            
            void AddTransform(Matrix4f& matrix) {m_Transforms.push_back(matrix);}
            void SetName(const std::string& name) {m_Name = name;}
            void SetName(std::string&& name) {m_Name = std::move(name);}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectTexture& obj);
    };

    template <typename T>
    struct ParameterValueMap
    {
        T Value;
        std::shared_ptr<SceneObjectTexture> ValueMap;

		ParameterValueMap() = default;
        ParameterValueMap(const T value) : Value(value) {}
        ParameterValueMap(const std::shared_ptr<SceneObjectTexture>& value) : ValueMap(value) {}
        
        ParameterValueMap(const ParameterValueMap<T>& rhs) = default;
        ParameterValueMap(ParameterValueMap<T>&& rhs) = default;
        
        ParameterValueMap& operator=(const ParameterValueMap<T>& rhs) = default;
        ParameterValueMap& operator=(ParameterValueMap<T>&& rhs) = default;
        ParameterValueMap& operator=(const std::shared_ptr<SceneObjectTexture>& rhs)
        {
            ValueMap = rhs;
            return *this;
        }

        ~ParameterValueMap() = default;

        friend std::ostream& operator<<(std::ostream& out, const ParameterValueMap<T>&obj)
        {
            out << "Parameter Value: " << obj.Value << std::endl;
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
            SceneObjectMaterial() : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(""), m_BaseColor(ColorRGBAf(1.0f)), m_Metallic(0.0f),
                m_Roughness(0.0f), m_Normal(Vector3Df(0.0f, 0.0f, 1.0f)), m_Specular(0.0f), m_AmbientOcclusion(1.0f)
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

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectMaterial& obj);
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
            const std::weak_ptr<SceneObjectMesh> GetMesh() {return m_Mesh.empty()? nullptr : m_Mesh[0];}
            const std::weak_ptr<SceneObjectMesh> GetMeshLOD(size_t lod) {return lod < m_Mesh.size()? m_Mesh[lod] : nullptr;}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectGeometry& obj);
    };

    typedef float (*AttenFunc) (float /* Intensity */, float /* Distance */);

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

            void SetColor(std::string& attrib, ColorRGBAf& color)
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

        protected:
            // can only used as base class of delivered lighting objects
            SceneObjectLight() : 
                BaseSceneObject(SceneObjectType::kSceneObjectTypeLight), m_LightColor(ColorRGBAf(1.0f)), m_Intensity(100.0f),
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

    // camera
    class SceneObjectCamera : public BaseSceneObject
    {
        protected:
            float m_Aspect;
            float m_NearClipDistance;
            float m_FarClipDistance;

        public:
            void SetColor(std::string& attrib, ColorRGBAf& color)
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
            }
            SceneObjectPerspectiveCamera(float fov = PI / 2.0f) :
                SceneObjectCamera(), m_Fov(fov)
                {}
            
            friend std::ostream& operator<<(std::ostream& out, const SceneObjectPerspectiveCamera& obj);
    };

    class SceneObjectTransform
    {
        protected:
            Matrix4f m_Matrix;
            bool    m_IsSceneObjectOnly;

        public:
            SceneObjectTransform() {m_Matrix.SetIdentity(); m_IsSceneObjectOnly = false;}
            SceneObjectTransform(const Matrix4f& matrix, const bool objectOnly = false) {m_Matrix = matrix; m_IsSceneObjectOnly = objectOnly;}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectTransform& obj);
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

#pragma once
#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "Guid.hpp"
#include "Image.hpp"
#include "portable.hpp"
#include "PandaMath.hpp"
#include "AssetLoader.hpp"
#include "JPEG.hpp"
#include "PNG.hpp"
#include "BMP.hpp"
#include "TGA.hpp"
#include "Curve.hpp"
#include "Animatable.hpp"

namespace Panda
{
    ENUM(SceneObjectType)
    {
		kSceneObjectTypeMesh = "MESH"_i32,
		kSceneObjectTypeMaterial = "MATL"_i32,
		kSceneObjectTypeTexture = "TXTU"_i32,
		kSceneObjectTypeLight = "LGHT"_i32,
		kSceneObjectTypeCamera = "CAMR"_i32,
		kSceneObjectTypeAnimationClip = "ANIM"_i32,
		kSceneObjectTypeClip = "CLIP"_i32,
		kSceneObjectTypeVertexArray = "VARR"_i32,
		kSceneObjectTypeIndexArray = "IARR"_i32,
		kSceneObjectTypeGeometry = "GEOM"_i32,
		kSceneObjectTypeTransform = "TRFM"_i32,
		kSceneObjectTypeTranslate = "TSLT"_i32,
		kSceneObjectTypeRotate = "ROTA"_i32,
		kSceneObjectTypeScale = "SCAL"_i32,
		kSceneObjectTypeTrack = "TRAC"_i32,
    };

    ENUM(SceneObjectCollisionType)
    {
        kSceneObjectCollisionTypeNone = "CNON"_i32,
        kSceneObjectCollisionTypeSphere = "CSPH"_i32,
        kSceneObjectCollisionTypeBox = "CBOX"_i32,
        kSceneObjectCollisionTypeCylinder = "CCYL"_i32,
        kSceneObjectCollisionTypeCapsule = "CCAP"_i32,
        kSceneObjectCollisionTypeCone = "CCON"_i32,
        kSceneObjectCollisionTypeMultiSphere = "CMUL"_i32,
        kSceneObjectCollisionTypeConvexHull = "CCVH"_i32,
        kSceneObjectCollisionTypeConvexMesh = "CCVM"_i32,
        kSceneObjectCollisionTypeBvhMesh = "CBVM"_i32,
        kSceneObjectCollisionTypeHeightfield = "CHIG"_i32,
        kSceneObjectCollisionTypePlane = "CPLN"_i32,
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

                const uint32_t GetMaterialIndex() const {return m_MaterialIndex;}
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

    public:
        SceneObjectMesh(bool visible = true, bool shadow = true, bool motion_blur = true) : 
            BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh)
        {}
        SceneObjectMesh(SceneObjectMesh&& mesh) :
            BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh),
            m_IndexArray(std::move(mesh.m_IndexArray)),
            m_VertexArray(std::move(mesh.m_VertexArray)),
            m_PrimitiveType(mesh.m_PrimitiveType)
            {}
        void AddIndexArray(SceneObjectIndexArray&& array) { m_IndexArray.push_back(std::move(array));}
        void AddVertexArray(SceneObjectVertexArray&& array) {m_VertexArray.push_back(std::move(array));}
        void SetPrimitiveType(PrimitiveType type) {m_PrimitiveType = type;}
        
		size_t GetIndexGroupCount() const { return m_IndexArray.size(); }
        size_t GetIndexCount(const size_t index) const {return m_IndexArray.empty()? 0 : m_IndexArray[index].GetIndexCount();}
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
            SceneObjectTexture() : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(0), m_pImage(nullptr) {}
			SceneObjectTexture(const std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_Name(name), m_TexCoordIndex(0), m_pImage(nullptr) {}
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(coord_index), m_pImage(image) {}
            SceneObjectTexture(uint32_t coord_index, std::shared_ptr<Image>&& image) : BaseSceneObject(SceneObjectType::kSceneObjectTypeTexture), m_TexCoordIndex(coord_index), m_pImage(std::move(image)) {}
            SceneObjectTexture(SceneObjectTexture&) = default;
            SceneObjectTexture(SceneObjectTexture&&) = default;
            
            void AddTransform(Matrix4f& matrix) {m_Transforms.push_back(matrix);}
            void SetName(const std::string& name) {m_Name = name;}
            void SetName(std::string&& name) {m_Name = std::move(name);}
            const std::string& GetName() const {return m_Name;}
            void LoadTexture()
            {
                if (!m_pImage)
                {
                    // we should lookup if the texture has been loaded already to prevent
                    // duplicate load. This could be done in Asset Loader Manager.
                    Buffer buf = g_pAssetLoader->SyncOpenAndReadBinary(m_Name.c_str());
                    std::string ext = m_Name.substr(m_Name.find_last_of("."));
                    if (ext == ".jpg" || ext == "jpeg")
                    {
                        JfifParser jfifParser;
                        m_pImage = std::make_shared<Image>(jfifParser.Parse(buf));                        
                    }
                    else if (ext == ".png")
                    {
                        PngParser pngParser;
                        m_pImage = std::make_shared<Image>(pngParser.Parse(buf));
                    }
                    else if (ext == ".bmp")
                    {
                        BmpParser bmpParser;
                        m_pImage = std::make_shared<Image>(bmpParser.Parse(buf));
                    }
                    else if (ext == ".tga")
                    {
                        TgaParser tgaParser;
                        m_pImage = std::make_shared<Image>(tgaParser.Parse(buf));
                    }
                }
            }

            const Image& GetTextureImage()
            {
                if (!m_pImage)
                {
                    LoadTexture();
                }

                return *m_pImage;
            }

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
            Color               m_Specular;
            Parameter           m_SpecularPower;
            Parameter           m_AmbientOcclusion;
            Color               m_Opacity;
            Color               m_Transparency;
            Color               m_Emission;

        public:
            SceneObjectMaterial() : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial),
                m_Name(""), m_BaseColor(Vector4Df(1.0f)), m_Metallic(0.0f), m_Roughness(0.0f), m_Normal(Vector3Df(0.0f, 0.0f, 1.0f)),
                m_Specular(0.0f), m_SpecularPower(1.0f), m_AmbientOcclusion(1.0f), m_Opacity(1.0f), m_Transparency(0.0f), m_Emission(0.0f) {}
            SceneObjectMaterial(const char* name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(name) {}
            SceneObjectMaterial(const std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(name) {}
            SceneObjectMaterial(std::string&& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(std::move(name)) {}

            const std::string& GetName() const {return m_Name;}
            const Color& GetBaseColor() const {return m_BaseColor;}
            const Color& GetSpecularColor() const {return m_Specular;}
            const Parameter& GetSpecularPower() const {return m_SpecularPower;}
            const Normal& GetNormal() const { return m_Normal; }
            void SetName(const std::string& name) {m_Name = name;}
            void SetName(std::string&& name) {m_Name = std::move(name);}
            void SetColor(const std::string& attrib, const ColorRGBAf& color)
            {
                if (attrib == "diffuse")
                {
                    m_BaseColor = Color(color);
                }
                else if (attrib == "specular")
                {
                    m_Specular = Color(color);
                }
                else if (attrib == "emission")
                {
                    m_Emission = Color(color);
                }
                else if (attrib == "opacity")
                {
                    m_Opacity = Color(color);
                }
                else if (attrib == "transparency")
                {
                    m_Opacity = Color(color);
                }
            }
            void SetParam(const std::string& attrib, float param)
            {
                if (attrib == "specular_power")
                {
                    m_SpecularPower = Parameter(param);
                }
            }

            void SetTexture(const std::string& attrib, const std::string& textureName)
            {
                if (attrib == "diffuse")
                {
                    m_BaseColor = std::make_shared<SceneObjectTexture>(textureName);
                }
                else if (attrib == "specular")
                {
                    m_Specular = std::make_shared<SceneObjectTexture>(textureName);
                }
                else if (attrib == "specular_power")
                {
                    m_SpecularPower = std::make_shared<SceneObjectTexture>(textureName);
                }
                else if (attrib == "emission")
                {
                    m_Emission = std::make_shared<SceneObjectTexture>(textureName);
                }
                else if (attrib == "opacity")
                {
                    m_Opacity = std::make_shared<SceneObjectTexture>(textureName);
                }
                else if (attrib == "transparency")
                {
                    m_Transparency = std::make_shared<SceneObjectTexture>(textureName);
                }
                else if (attrib == "normal")
                {
                    m_Normal = std::make_shared<SceneObjectTexture>(textureName);
                }
            }

            void SetTexture(const std::string& attrib, const std::shared_ptr<SceneObjectTexture>& texture)
            {
                if (attrib == "diffuse")
                {
                    m_BaseColor = texture;
                }
                else if (attrib == "specular")
                {
                    m_Specular = texture;
                }
                else if (attrib == "specular_power")
                {
                    m_SpecularPower = texture;
                }
                else if (attrib == "opacity")
                {
                    m_Opacity = texture;
                }
                else if (attrib == "transparency")
                {
                    m_Transparency = texture;
                }
                else if (attrib == "normal")
                {
                    m_Normal = texture;
                }
            }

            void LoadTexture()
            {
                if (m_BaseColor.ValueMap)
                {
                    m_BaseColor.ValueMap->LoadTexture();
                }
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
            SceneObjectCollisionType m_CollisionType;
            float       m_CollisionParameters[10];

        public:
            SceneObjectGeometry() : BaseSceneObject(SceneObjectType::kSceneObjectTypeGeometry), m_CollisionType(SceneObjectCollisionType::kSceneObjectCollisionTypeNone) {}

            void SetVisibility(bool visible) {m_IsVisible = visible;}
            const bool Visible() {return m_IsVisible;}
            void SetIfCastShadow(bool shadow) {m_IsShadow = shadow;}
            const bool CastShadow() {return m_IsShadow;}
            void SetIfMotionBlur(bool motionBlur) {m_IsMotionBlur = motionBlur;}
            const bool MotionBlur() {return m_IsMotionBlur;}
            void SetCollisionType(SceneObjectCollisionType collisionType) {m_CollisionType = collisionType;}
            const SceneObjectCollisionType CollisionType() const {return m_CollisionType;}
            void SetCollisionParameters(const float* param, int32_t count)
            {
                assert(count > 0 && count < 10);
                memcpy(m_CollisionParameters, param, sizeof(float) * count);
            }
            const float* CollisionParameters() const { return m_CollisionParameters;}

            void AddMesh(std::shared_ptr<SceneObjectMesh>& mesh) {m_Mesh.push_back(std::move(mesh));}
            const std::weak_ptr<SceneObjectMesh> GetMesh() {return m_Mesh.empty()? nullptr : m_Mesh[0];}
            const std::weak_ptr<SceneObjectMesh> GetMeshLOD(size_t lod) {return lod < m_Mesh.size()? m_Mesh[lod] : nullptr;}

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectGeometry& obj);
    };

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

            void SetAttenuation(AttenFunc func)
            {
                m_LightAttenuation = func;
            }

            const Color& GetColor() {return m_LightColor;}
            float GetIntensity() {return m_Intensity;}

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

	// infinite light
	class SceneObjectInfiniteLight : public SceneObjectLight
	{
	public:
		using SceneObjectLight::SceneObjectLight;

		friend std::ostream& operator<<(std::ostream& out, const SceneObjectInfiniteLight& obj);
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

    class SceneObjectTransform : public BaseSceneObject
    {
        protected:
            Matrix4f m_Matrix;
            bool    m_IsSceneObjectOnly;

        public:
            SceneObjectTransform() : BaseSceneObject(SceneObjectType::kSceneObjectTypeTransform) 
			{m_Matrix.SetIdentity(); m_IsSceneObjectOnly = false;}
            SceneObjectTransform(const Matrix4f& matrix, const bool objectOnly = false) : SceneObjectTransform() 
			{m_Matrix = matrix; m_IsSceneObjectOnly = objectOnly;}

            operator Matrix4f() {return m_Matrix;}
            operator const Matrix4f() const {return m_Matrix;}
            friend std::ostream& operator<<(std::ostream& out, const SceneObjectTransform& obj);
    };

    class SceneObjectTranslation : public SceneObjectTransform, implements Animatable<float>
    {
        private:
            char m_Kind = 0;

        public:
			SceneObjectTranslation() { m_Type = SceneObjectType::kSceneObjectTypeTranslate; }
            SceneObjectTranslation(const char axis, const float amount)
				:SceneObjectTranslation()
            {
                m_Kind = axis;

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
            SceneObjectTranslation(const float x, const float y, const float z, const bool object_only = false)
				:SceneObjectTranslation()
            {
                m_Kind = 0;
                MatrixTranslation(m_Matrix, x, y, z);

                m_IsSceneObjectOnly = object_only;
            }

            void Update(const float amount) final
            {
                switch (m_Kind)
                {
                    case 'x':
                        MatrixTranslation(m_Matrix, amount, 0.0f, 0.0f);
                        break;
                    case 'y':
                        MatrixTranslation(m_Matrix, 0.0f, amount, 0.0f);
                        break;
                    case 'z':
                        MatrixTranslation(m_Matrix, 0.0f, 0.0f, amount);
                        break;
                    default:
                        assert(0);
                }
            }
    };

    class SceneObjectRotation : public SceneObjectTransform, implements Animatable<float>
    {
        private:
            char m_Kind = 0;

        public:
			SceneObjectRotation() { m_Type = SceneObjectType::kSceneObjectTypeRotate; }
            SceneObjectRotation(const char axis, const float theta, const bool object_only = false)
				:SceneObjectRotation()
            {
                m_Kind = axis;
                m_IsSceneObjectOnly = object_only;

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

            SceneObjectRotation(Vector3Df& axis, const float theta, const bool object_only = false)
				:SceneObjectRotation()
            {
                m_Kind = 0;
				axis.Normalize();
                MatrixRotationAxis(m_Matrix, axis, theta);
                m_IsSceneObjectOnly = object_only;
            }

            SceneObjectRotation(const Quaternion& q, const bool object_only = false)
				:SceneObjectRotation()
            {
                m_Kind = 0;
                MatrixRotationQuaternion(m_Matrix, q);
                m_IsSceneObjectOnly = object_only;
            }

            void Update(const float theta) final
            {
                switch(m_Kind)
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
                        assert(0);
                }
            }
    };

    class SceneObjectScale : public SceneObjectTransform, implements Animatable<float>
    {
        private:
            char m_Kind = 0;

        public:
			SceneObjectScale() { m_Type = SceneObjectType::kSceneObjectTypeScale; }
        SceneObjectScale(const char axis, const float amount)
			:SceneObjectScale()
        {
            m_Kind = axis;
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
			:SceneObjectScale()
        {
            m_Kind = 0;
            MatrixScale(m_Matrix, x, y, z);
        }

        void Update(const float amount) final
        {
            switch(m_Kind)
            {
                case 'x':
                    MatrixScale(m_Matrix, amount, 0.0f, 0.0f);
                    break;
                case 'y':
                    MatrixScale(m_Matrix, 0.0f, amount, 0.0f);
                    break;
                case 'z':
                    MatrixScale(m_Matrix, 0.0f, 0.0f, amount);
                    break;
                default:
                    assert(0);
            }
        }
    };

    class SceneObjectTrack : public BaseSceneObject, implements Animatable<float>
    {
        private:
            std::shared_ptr<SceneObjectTransform> m_pTransform;
            std::shared_ptr<Curve<float>> m_Time;
            std::shared_ptr<Curve<float>> m_Value;

        public:
            SceneObjectTrack() = delete;
			SceneObjectTrack(std::shared_ptr<SceneObjectTransform>& trans,
				std::shared_ptr<Curve<float>>& time,
				std::shared_ptr<Curve<float>>& value)
				: BaseSceneObject(SceneObjectType::kSceneObjectTypeTrack),
				m_pTransform(trans), m_Time(time), m_Value(value)
            {}

            void Update(const float timePoint) final;

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectTrack& obj);
    };

    class SceneObjectAnimationClip : public BaseSceneObject, implements Animatable<float>
    {
        private:
            int m_Index = 0;
            bool m_IsLoop = false;
            std::vector<std::shared_ptr<SceneObjectTrack>> m_Tracks;

        public:
            SceneObjectAnimationClip() = delete;
            SceneObjectAnimationClip(int index) : BaseSceneObject(SceneObjectType::kSceneObjectTypeAnimationClip),
                m_Index(index)
            {}
            int GetIndex() {return m_Index;}

            void AddTrack(std::shared_ptr<SceneObjectTrack>& track);
            void Update(const float timePoint) final;

            friend std::ostream& operator<<(std::ostream& out, const SceneObjectAnimationClip& obj);
    };
}

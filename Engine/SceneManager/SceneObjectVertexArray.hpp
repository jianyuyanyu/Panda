#pragma once
#include <string>
#include "SceneObjectTypeDef.hpp"

namespace Panda
{
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
}
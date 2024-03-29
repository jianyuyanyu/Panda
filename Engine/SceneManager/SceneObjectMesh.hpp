#pragma once
#include <vector>
#include "SceneObjectIndexArray.hpp"
#include "SceneObjectVertexArray.hpp"
#include "Math/PandaMath.hpp"
#include "BaseSceneObject.hpp"

namespace Panda
{
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
}
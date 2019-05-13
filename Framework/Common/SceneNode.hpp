#pragma once
#include <iostream>
#include <list>
#include <memory>
#include <vector>
#include <string>
#include "SceneObject.hpp"

namespace Panda
{
    class BaseSceneNode
    {
        protected:
            std::string m_Name;
            std::list<std::unique_ptr<BaseSceneNode>> m_Children;
            std::list<std::unique_ptr<SceneObjectTransform>> m_Transforms;

        protected:
            virtual void Dump(std::ostream& out) const {}

        public:
            BaseSceneNode() {}
            BaseSceneNode(const char* name) {m_Name = name;}
            BaseSceneNode(const std::string& name) {m_Name = name;}
            BaseSceneNode(const std::string&& name) {m_Name = std::move(name);}
            virtual ~BaseSceneNode() {}

            void AppendChild(std::unique_ptr<BaseSceneNode>&& node)
            {
                m_Children.push_back(std::move(node));
            }

            void AppendTransform(std::unique_ptr<SceneObjectTransform>&& transform)
            {
                m_Transforms.push_back(std::move(transform));
            }

            friend std::ostream& operator<<(std::ostream& out, const BaseSceneNode& node)
            {
                static thread_local int32_t indent = 0;
                indent++;

                out << std::string(indent, ' ') << "Scene Node" << std::endl;
                out << std::string(indent, ' ') << "----------" << std::endl;
                out << std::string(indent, ' ') << "Name: " << node.m_Name << std::endl;
                node.Dump(out);
                out << std::endl;

                for (const std::unique_ptr<BaseSceneNode>& subNode : node.m_Children)
                {
                    out << *subNode << std::endl;
                }

                for (const std::unique_ptr<SceneObjectTransform>& subTransform : node.m_Transforms)
                {
                    out << *subTransform << std::endl;
                }

                indent--;

                return out;
            }
	};

    template <typename T>
    class SceneNode : public BaseSceneNode
    {
        protected:
            std::string m_SceneObjectKey;

        protected:
            virtual void Dump(std::ostream& out) const
            {
                out << m_SceneObjectKey << std::endl;
            }

        public:
            using BaseSceneNode::BaseSceneNode;
            SceneNode() = default;

            void AddSceneObjectRef(const std::string& key) {m_SceneObjectKey = key;}
    };

    typedef BaseSceneNode SceneEmptyNode;

    class SceneGeometryNode : public SceneNode<SceneObjectGeometry>
    {
        protected:
            bool    m_IsVisible;
            bool    m_IsShadow;
            bool    m_IsMotionBlur;
            std::vector<std::string> m_Materials;

        protected:
            virtual void Dump(std::ostream& out)
            {
                SceneNode::Dump(out);
                out << "Visible: " << m_IsVisible << std::endl;
                out << "Shadow: " << m_IsShadow << std::endl;
                out << "Motion Blur: " << m_IsMotionBlur << std::endl;
                out << "Material(s): " << std::endl;
                for (auto material : m_Materials)
                {
                    out << material << std::endl;
                }
            }

        public:
            using SceneNode::SceneNode;

            void SetVisibility(bool visible) {m_IsVisible = visible;}
            const bool Visible() {return m_IsVisible;}
            void SetIfCastShadow(bool shadow) {m_IsShadow = shadow;}
            const bool CastShadow() {return m_IsShadow;}
            void SetIfMotionBlur(bool motionBlur) {m_IsMotionBlur = motionBlur;}
            const bool MotionBlur() {return m_IsMotionBlur;}
            using SceneNode::AddSceneObjectRef;
            void AddMaterialRef(const std::string& key) {m_Materials.push_back(key);}
            void AddMaterialRef(std::string&& key) {m_Materials.push_back(std::move(key));}
    };
    
    class SceneLightNode : public SceneNode<SceneObjectLight>
    {
        protected:
            bool    m_IsShadow;

        public:
            using SceneNode::SceneNode;

            void SetIfCastShadow(bool shadow) {m_IsShadow = shadow;}
            const bool CastShadow() {return m_IsShadow;}
    };
    
    class SceneCameraNode : public SceneNode<SceneObjectCamera>
    {
        protected:
            Vector3Df m_Target;

        public:
            using SceneNode::SceneNode;

            void SetTarget(Vector3Df& target) {m_Target = target;}
            const Vector3Df& GetTarget() {return m_Target;}
    };
}

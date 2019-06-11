#pragma once
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include "Tree.hpp"
#include "SceneObject.hpp"

namespace Panda
{
    class BaseSceneNode : public TreeNode
    {
        protected:
            std::string m_Name;
            std::vector<std::shared_ptr<SceneObjectTransform>> m_Transforms;
            std::map<int, std::shared_ptr<SceneObjectAnimationClip>> m_AnimationClips;
            std::map<std::string, std::shared_ptr<SceneObjectTransform>> m_LUTransform;
            
			Matrix4f m_RuntimeTransform;

        protected:
            virtual void Dump(std::ostream& out) const {}

        public:
            typedef std::map<int, std::shared_ptr<SceneObjectAnimationClip>>::const_iterator AnimationClipIterator;

        public:
            BaseSceneNode() {m_RuntimeTransform.SetIdentity();}
            BaseSceneNode(const std::string& name) {m_Name = name; m_RuntimeTransform.SetIdentity();}
            virtual ~BaseSceneNode() {}

            const std::string GetName() const {return m_Name;}

			void AttachAnimationClip(int clipIndex, std::shared_ptr<SceneObjectAnimationClip> clip)
            {
                m_AnimationClips.insert({clipIndex, clip});
            }

            inline bool GetFirstAnimationClip(AnimationClipIterator& it)
            {
                it = m_AnimationClips.cbegin();
                return it != m_AnimationClips.cend();
            }

            inline bool GetNextAnimationClip(AnimationClipIterator& it)
            {
                it++;
                return it != m_AnimationClips.cend();
            }

			void AppendTransform(const char* key, const std::shared_ptr<SceneObjectTransform>& transform)
			{
				m_Transforms.push_back(transform);
                m_LUTransform.insert({std::string(key), transform});
			}

            std::shared_ptr<SceneObjectTransform> GetTransform(const std::string& key)
            {
                auto it = m_LUTransform.find(key);
                if (it != m_LUTransform.end())
                {
                    return it->second;
                }
                else
                {
                    return std::shared_ptr<SceneObjectTransform>();
                }
            }

            const std::shared_ptr<Matrix4f> GetCalculatedTransform() const
            {
                std::shared_ptr<Matrix4f> result (new Matrix4f);
                result->SetIdentity();

                // TODO: cascading calcuation
                for (auto it = m_Transforms.rbegin(); it != m_Transforms.rend(); ++it)
                {
                    *result = *result * static_cast<Matrix4f>(**it);
                }

                // apply runtime transforms
                *result = *result * m_RuntimeTransform;

                return result;
            }

            void RotateBy(float rotationAngleX, float rotationAngleY, float rotationAngleZ)
            {
                Matrix4f rotate;
                MatrixRotationYawPitchRoll(rotate, rotationAngleX, rotationAngleY, rotationAngleZ);
                m_RuntimeTransform = m_RuntimeTransform * rotate;
            }

            void MoveBy(float distanceX, float distanceY, float distanceZ)
            {
                Matrix4f translation;
                MatrixTranslation(translation, distanceX, distanceY, distanceZ);
                m_RuntimeTransform = m_RuntimeTransform * translation;
            }

            void MoveBy(const Vector3Df& distance)
            {
                MoveBy(distance.data[0], distance.data[1], distance.data[2]);
            }

            virtual Matrix3f GetLocalAxis()
            {
                return {
                    {1.0f, 0.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f}
                };
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

                for (auto subNode : node.m_Children)
                {
                    out << *subNode << std::endl;
                }

                for (auto subTransform : node.m_Transforms)
                {
                    out << *subTransform << std::endl;
                }

                for (auto animClip : node.m_AnimationClips)
                {
                    out << * animClip.second << std::endl;
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
            const std::string& GetSceneObjectRef() {return m_SceneObjectKey;}
    };

    typedef BaseSceneNode SceneEmptyNode;

    class SceneGeometryNode : public SceneNode<SceneObjectGeometry>
    {
        protected:
            bool    m_IsVisible;
            bool    m_IsShadow;
            bool    m_IsMotionBlur;
            std::vector<std::string> m_Materials;
            void*   m_pRigidBody = nullptr;

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
            std::string GetMaterialRef(const size_t index)
            {
                if (index < m_Materials.size())
                    return m_Materials[index];
                else 
                    return std::string("default");
            }

            void LinkRigidBody(void* pRigidBody)
            {
                m_pRigidBody = pRigidBody;
            }

            void* UnlinkRigidBody()
            {
                void* pRigidBody = m_pRigidBody;
                m_pRigidBody = nullptr;

                return pRigidBody;
            }

            void* RigidBody() {return m_pRigidBody;}
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

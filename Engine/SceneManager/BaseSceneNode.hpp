#pragma once
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include "Math/Tree.hpp"
#include "SceneObject.hpp"

namespace Panda
{
    class BaseSceneNode : public TreeNode
    {
        protected:
			std::string m_Sid;
            std::string m_Name;
            std::vector<std::shared_ptr<SceneObjectTransform>> m_Transforms;
            std::map<std::string, std::shared_ptr<SceneObjectTransform>> m_LUTransform;
            
			Matrix4f m_RuntimeTransform;

        protected:
            virtual void Dump(std::ostream& out) const {}

        public:
            BaseSceneNode() {m_RuntimeTransform.SetIdentity();}
            BaseSceneNode(const std::string& name) {
				m_Name = name; 
				m_Sid = name;
				m_RuntimeTransform.SetIdentity();
			}
			BaseSceneNode(const std::string& name, const std::string& sid)
			{
				m_Name = name;
				m_Sid = sid;
				m_RuntimeTransform.SetIdentity();
			}
            virtual ~BaseSceneNode() {}

            const std::string GetName() const {
				return m_Name;
			}

			const std::string GetSid() const
			{
				return m_Sid;
			}

			virtual BaseSceneNode* GetParent()
			{
				return reinterpret_cast<BaseSceneNode*>(m_Parent);
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
                return Matrix3f({
                    1.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 1.0f
                });
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
}
#pragma once

#include <unordered_map>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "GraphicsManager.hpp"
#include "PandaMath.hpp"
#include "glad/glad.h"
#include "SceneObject.hpp"

namespace Panda {
    class OpenGLGraphicsManager : public GraphicsManager
    {
        public:
            virtual int Initialize();
            virtual void Finalize();

            virtual void Clear();
            virtual void Draw();

        protected:
            bool SetPerBatchShaderParameters(const char* paramName, const Matrix4f& param);
            bool SetPerBatchShaderParameters(const char* paramName, const float param);
            bool SetPerBatchShaderParameters(const char* paramName, const Vector3Df& param);
            bool SetPerBatchShaderParameters(const char* paramName, const int param);
            bool SetPerFrameShaderParameters();
            
            //bool SetShaderParameters(const Matrix4f& worldMatrix, const Matrix4f& viewMatrix, const Matrix4f& projectionMatrix);

            void InitializeBuffers();
            void RenderBuffers();
            bool InitializeShader(const char* vsFileName, const char* fsFileName);

        private:
            unsigned int m_VertexShader;
            unsigned int m_FragmentShader;
            unsigned int m_ShaderProgram;
            std::map<std::string, GLint> m_TextureIndex;

            struct DrawBatchContext
            {
                GLuint vao;
                GLenum mode;
                GLenum type;
                GLsizei count;
                std::shared_ptr<Matrix4f> transform;
                std::shared_ptr<SceneObjectMaterial> material;

				friend std::ostream& operator<<(std::ostream& out, DrawBatchContext context)
				{
					out << std::endl;
					out << "------------------" << std::endl;
					out << "vao = " << context.vao << std::endl;
					out << "mode = " << context.mode << std::endl;
					out << "type = " << context.type << std::endl;
					out << "count = " << context.count << std::endl;
					out << "transform = " << *context.transform << std::endl;
					if (context.material)
						out << "material = " << *context.material << std::endl;
					else
						out << "material = nullptr" << std::endl;
					out << "------------------" << std::endl;
					out << std::endl;
					return out;
				}
            };

            std::vector<DrawBatchContext> m_DrawBatchContext;
            std::vector<GLuint> m_Buffers;
            std::vector<GLuint> m_Textures;
    };
}
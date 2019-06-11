#pragma once

#include <unordered_map>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "GraphicsManager.hpp"
#include "PandaMath.hpp"
#include "glad/glad.h"
#include "SceneManager.hpp"

namespace Panda {
    class OpenGLGraphicsManager : public GraphicsManager
    {
        public:
            virtual int Initialize();
            virtual void Finalize();

            virtual void Clear();
            virtual void Draw();

            #ifdef DEBUG
            void DrawLine(const Point& from, const Point& to, const Vector3Df& color) final;
            void DrawLine(const PointList& vertices, const Vector3Df& color) final;
            void DrawLine(const PointList& vertices, const Matrix4f& trans, const Vector3Df& color) final;
            void ClearDebugBuffers();
            #endif

        protected:
            bool SetPerBatchShaderParameters(GLuint shader, const char* paramName, const Matrix4f& param);
            bool SetPerBatchShaderParameters(GLuint shader, const char* paramName, const float param);
            bool SetPerBatchShaderParameters(GLuint shader, const char* paramName, const Vector3Df& param);
            bool SetPerBatchShaderParameters(GLuint shader, const char* paramName, const int param);
            bool SetPerFrameShaderParameters(GLuint shader );
            
            //bool SetShaderParameters(const Matrix4f& worldMatrix, const Matrix4f& viewMatrix, const Matrix4f& projectionMatrix);

            // void InitializeBuffers();
            // void RenderBuffers();
            // bool InitializeShader(const char* vsFileName, const char* fsFileName);

            void InitializeBuffers(const Scene& scene);
            void ClearBuffers();
            bool InitializeShaders();
            void ClearShaders();
            void RenderBuffers();

        private:
            GLuint m_VertexShader;
            GLuint m_FragmentShader;
            GLuint m_ShaderProgram;

#ifdef DEBUG
            GLuint m_debugVertexShader;
            GLuint m_debugFragmentShader;
            GLuint m_debugShaderProgram;
#endif
            std::map<std::string, GLint> m_TextureIndex;

            struct DrawBatchContext
            {
                GLuint vao;
                GLenum mode;
                GLenum type;
                GLsizei count;
                std::shared_ptr<SceneGeometryNode> node;
                std::shared_ptr<SceneObjectMaterial> material;

				friend std::ostream& operator<<(std::ostream& out, DrawBatchContext context)
				{
					out << std::endl;
					out << "------------------" << std::endl;
					out << "vao = " << context.vao << std::endl;
					out << "mode = " << context.mode << std::endl;
					out << "type = " << context.type << std::endl;
					out << "count = " << context.count << std::endl;
					//out << "node = " << *node << std::endl;
					if (context.material)
						out << "material = " << *context.material << std::endl;
					else
						out << "material = nullptr" << std::endl;
					out << "------------------" << std::endl;
					out << std::endl;
					return out;
				}
            };

#ifdef DEBUG
            struct DebugDrawBatchContext {
                GLuint  vao;
                GLenum  mode;
                GLsizei count;
                Vector3Df color;
                Matrix4f trans;
            };
#endif

            std::vector<DrawBatchContext> m_DrawBatchContext;
            std::vector<GLuint> m_Buffers;
            std::vector<GLuint> m_Textures;

#ifdef DEBUG
            std::vector<DebugDrawBatchContext> m_DebugDrawBatchContext;
            std::vector<GLuint> m_DebugBuffers;
#endif
    };
}
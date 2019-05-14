#pragma once
#include "GraphicsManager.hpp"
#include "PandaMath.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "glad/glad.h"

namespace Panda {
    class OpenGLGraphicsManager : public GraphicsManager
    {
        public:
            virtual int Initialize();
            virtual void Finalize();

            virtual void Tick();

            virtual void Clear();
            virtual void Draw();
        private:
            bool SetPerBatchShaderParameters(const char* paramName, float* param);
            bool SetPerFrameShaderParameters();
            
            //bool SetShaderParameters(const Matrix4f& worldMatrix, const Matrix4f& viewMatrix, const Matrix4f& projectionMatrix);

            void InitializeBuffers();
            void RenderBuffers();
            void CalculateCameraMatrix();
            void CalculateLights();
            bool InitializeShader(const char* vsFileName, const char* fsFileName);

        private:
            unsigned int m_VertexShader;
            unsigned int m_FragmentShader;
            unsigned int m_ShaderProgram;

            struct DrawFrameContext
            {
                Matrix4f    WorldMatrix;
                Matrix4f    ViewMatrix;
                Matrix4f    ProjectionMatrix;
                Vector3Df   LightPosition;
                Vector4Df   LightColor;
            };
            const bool VSYNC_ENABLED = true;
            const float k_ScreenFar = 1000.f;
            const float k_ScreenNear = 0.1f;

            struct DrawBatchContext
            {
                GLuint vao;
                GLenum mode;
                GLenum type;
                GLsizei count;
                std::shared_ptr<Matrix4f> transform;
            };

            DrawFrameContext m_DrawFrameContext;
            std::vector<DrawBatchContext> m_DrawBatchContext;
            std::vector<GLuint> m_Buffers;
    };
}
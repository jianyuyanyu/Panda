#pragma once
#include "GraphicsManager.hpp"
#include "PandaMath.hpp"

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
            bool SetShaderParameters(const Matrix4f& worldMatrix, const Matrix4f& viewMatrix, const Matrix4f& projectionMatrix);

            bool InitializeBuffers();
            void RenderBuffers();
            void CalculateCameraPosition();
            bool InitializeShader(const char* vsFileName, const char* fsFileName);

        private:
            unsigned int m_VertexShader;
            unsigned int m_FragmentShader;
            unsigned int m_ShaderProgram;

            const bool VSYNC_ENABLED = true;
            const float k_ScreenFar = 1000.f;
            const float k_ScreenNear = 0.1f;

            int m_VertexCount, m_IndexCount;
            unsigned int m_VertexArrayId, m_VertexBufferId, m_IndexBufferId;

            float m_PositionX = 0, m_PositionY = 0, m_PositionZ = -10;
            float m_RotationX = 0, m_RotationY = 0, m_RotationZ = 0;
            Matrix4f m_WorldMatrix;
            Matrix4f m_ViewMatrix;
            Matrix4f m_ProjectionMatrix;
    };
}
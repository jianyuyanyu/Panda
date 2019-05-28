#pragma once
#include "PandaMath.hpp"
#include "Image.hpp"
#include "IRuntimeModule.hpp"

namespace Panda {
	class GraphicsManager : implements IRuntimeModule {
		public:
			virtual ~GraphicsManager() {}

			virtual int Initialize();
			virtual void Finalize();

			virtual void Tick();

			virtual void Clear();
			virtual void Draw();

			// temporary. It should be moved to scene manager and script engine(policy engine)
			void WorldRotateX(float radians);
			void WorldRotateY(float radians);

		protected:
			bool SetPerFrameShaderParameters();
			bool SetPerBatchShaderParameters(const char* paramName, const Matrix4f& param);
			bool SetPerBatchShaderParameters(const char* paramName, const Vector3Df& param);
			bool SetPerBatchShaderParameters(const char* paramName, const float param);
			bool SetPerBatchShaderParameters(const char* paramName, const int param);

			void InitConstants();
			bool InitializeShader(const char* vsFilename, const char* psFilename);
			void InitializeBuffers();
			void CalculateCameraMatrix();
			void CalculateLights();
			void RenderBuffers();

		protected:
			struct DrawFrameContext
			{
				Matrix4f WorldMatrix;
				Matrix4f ViewMatrix;
				Matrix4f ProjectionMatrix;
				Vector3Df LightPosition;
				Vector4Df LightColor;

				friend std::ostream& operator<<(std::ostream& out, DrawFrameContext context)
				{
					out << std::endl;
					out << "----------------------" << std::endl;
					out << "WorldMatrix = " << context.WorldMatrix << std::endl;
					out << "ViewMatrix = " << context.ViewMatrix << std::endl;
					out << "ProjectionMatrix = " << context.ProjectionMatrix << std::endl;
					out << "LightPosition = " << context.LightPosition << std::endl;
					out << "LightColor = " << context.LightColor << std::endl;
					out << "----------------------" << std::endl;
					out << std::endl;

					return out;
				}
			};

			DrawFrameContext m_DrawFrameContext;
	};

	extern GraphicsManager* g_pGraphicsManager;
}

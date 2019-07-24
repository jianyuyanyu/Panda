#pragma once

#include "Interface/IRuntimeModule.hpp"
#include "Math/PandaMath.hpp"
#include "Image.hpp"
#include "Scene.hpp"

namespace Panda {
	class GraphicsManager : implements IRuntimeModule {
		public:
			virtual ~GraphicsManager() {}

			virtual int Initialize();
			virtual void Finalize();

			virtual void Tick();

			virtual void Clear();
			virtual void Draw();

			#ifdef DEBUG
			virtual void DrawLine(const Point& from, const Point& to, const Vector3Df& color);
			virtual void DrawLine(const PointList& vertices, const Vector3Df& color);
            virtual void DrawLine(const PointList& vertices, const Matrix4f& trans, const Vector3Df& color);
			virtual void ClearDebugBuffers();
			#endif

		protected:
			virtual bool InitializeShaders();
			virtual void ClearShaders();
			virtual void InitializeBuffers(const Scene& scene);
			virtual void ClearBuffers();

			virtual void InitConstants();
			virtual void CalculateCameraMatrix();
			virtual void CalculateLights();
			virtual void UpdateConstants();
			virtual void RenderBuffers();

		protected:
			struct LightContext
			{
				Vector4Df LightPosition;
				Vector4Df LightColor;
				Vector4Df LightDirection;
				float LightIntensity;
				AttenCurveType LightDistAttenCurveType;
				float LightDistAttenCurveParams[5];
				AttenCurveType LightAngleAttenCurveType;
				float LightAngleAttenCurveParams[5];
			};

			struct DrawFrameContext
			{
				Matrix4f WorldMatrix;
				Matrix4f ViewMatrix;
				Matrix4f ProjectionMatrix;
				Vector3Df AmbientColor;
				std::vector<LightContext> Lights;

				//friend std::ostream& operator<<(std::ostream& out, DrawFrameContext context)
				//{
				//	out << std::endl;
				//	out << "----------------------" << std::endl;
				//	out << "WorldMatrix = " << context.WorldMatrix << std::endl;
				//	out << "ViewMatrix = " << context.ViewMatrix << std::endl;
				//	out << "ProjectionMatrix = " << context.ProjectionMatrix << std::endl;
				//	out << "----------------------" << std::endl;
				//	out << std::endl;

				//	return out;
				//}
			};

			DrawFrameContext m_DrawFrameContext;
	};

	extern GraphicsManager* g_pGraphicsManager;
}

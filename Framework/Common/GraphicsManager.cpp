#include <iostream>
#include "GraphicsManager.hpp"
#include "SceneManager.hpp"
#include "IApplication.hpp"

namespace Panda
{
	int GraphicsManager::Initialize()
	{
		int result = 0;
		InitConstants();
		return result;
	}

	void GraphicsManager::Finalize()
	{
		#ifdef DEBUG
		ClearDebugBuffers();
		#endif
		ClearBuffers();
		ClearShaders();
	}

	void GraphicsManager::Tick()
	{
		if (g_pSceneManager->IsSceneChanged())
		{
			std::cout << "[GraphicsManager] Detected Scene Change, reinitialize buffers ..." << std::endl;
			ClearBuffers();
			ClearShaders();

			const Scene& scene = g_pSceneManager->GetScene();
			InitializeShaders();
			InitializeBuffers(scene);
			g_pSceneManager->NotifySceneIsRenderingQueued();
		}

		UpdateConstants();

		Clear();
		Draw();
		//std::cout << m_DrawFrameContext;
	}

	void GraphicsManager::UpdateConstants()
	{
		// Generate teh view matrix based on the camera's position.
		CalculateCameraMatrix();
		CalculateLights();
	}

	void GraphicsManager::Clear()
	{

	}

	void GraphicsManager::Draw()
	{
		UpdateConstants();

		RenderBuffers();
	}

	void GraphicsManager::InitConstants()
	{
		// Initialize the world/model matrix to the identity matrix.
		m_DrawFrameContext.WorldMatrix.SetIdentity();
	}

	bool GraphicsManager::InitializeShaders()
	{
		std::cout << "[GraphicsManager] GraphicsManager::InitializeShaders()" << std::endl;
		return true;
	}

	void GraphicsManager::ClearShaders()
	{
		std::cout << "[GraphicsManager] GraphicsManager::ClearShaders()" << std::endl;
	}

	void GraphicsManager::CalculateCameraMatrix()
	{
		auto& scene = g_pSceneManager->GetScene();
		auto pCameraNode = scene.GetFirstCameraNode();
		if (pCameraNode)
		{
			m_DrawFrameContext.ViewMatrix = *pCameraNode->GetCalculatedTransform();
			InverseMatrix(m_DrawFrameContext.ViewMatrix, m_DrawFrameContext.ViewMatrix);
		}
		else 
		{
			// use default camera
			Vector3Df position({0.0f, -5.0f, 0.0f}), lookAt({0.0f, 0.0f, 0.0f}), up({0.0f, 0.0f, 1.0f});
			BuildViewMatrix(m_DrawFrameContext.ViewMatrix, position, lookAt, up);
		}

		float fieldOfView = PI / 2.0f;
		float nearClipDistance = 1.0f;
		float farClipDistance = 100.0f;

		if (pCameraNode)
		{
			auto pCamera = scene.GetCamera(pCameraNode->GetSceneObjectRef());
			// Set the field of view and screen aspect ratio.
			fieldOfView = std::dynamic_pointer_cast<SceneObjectPerspectiveCamera>(pCamera)->GetFov();
			nearClipDistance = pCamera->GetNearClipDistance();
			farClipDistance = pCamera->GetFarClipDistance();
		}

		const GfxConfiguration& conf = g_pApp->GetConfiguration();
		float screenAspect = (float)conf.screenWidth / conf.screenHeight;

		// Build the perspective projection matrix.
		BuildPerspectiveFovMatrix(m_DrawFrameContext.ProjectionMatrix, fieldOfView, screenAspect, nearClipDistance, farClipDistance);
	}

	void GraphicsManager::CalculateLights()
	{
		m_DrawFrameContext.AmbientColor = {0.01f, 0.01f, 0.01f};
		auto& scene = g_pSceneManager->GetScene();
		auto pLightNode = scene.GetFirstLightNode();
		if (pLightNode)
		{
			const std::shared_ptr<Matrix4f> transPtr = pLightNode->GetCalculatedTransform();
			m_DrawFrameContext.LightPosition = {0.0f, 0.0f, 0.0f, 1.0f};
			TransformCoord(m_DrawFrameContext.LightPosition, *transPtr);
			m_DrawFrameContext.LightDirection = {0.0f, 0.0f, -1.0f};
			TransformCoord(m_DrawFrameContext.LightDirection, *transPtr);

			const std::shared_ptr<SceneObjectLight> pLight = scene.GetLight(pLightNode->GetSceneObjectRef());
			if (pLight)
			{
				m_DrawFrameContext.LightColor = pLight->GetColor().Value;
				m_DrawFrameContext.LightIntensity = pLight->GetIntensity();
				const AttenCurve& attenCurve = pLight->GetDistanceAttenuation();
				m_DrawFrameContext.LightDistAttenCurveType = attenCurve.type;
				memcpy(m_DrawFrameContext.LightDistAttenCurveParams, &attenCurve.u, sizeof(attenCurve.u));
				if (pLight->GetType() == SceneObjectType::kSceneObjectTypeLightSpot)
				{
					const std::shared_ptr<SceneObjectSpotLight> _pLight = std::dynamic_pointer_cast<SceneObjectSpotLight>(pLight);
					const AttenCurve& angleAttenCurve = _pLight->GetAngleAttenuation();
					m_DrawFrameContext.LightAngleAttenCurveType = angleAttenCurve.type;
					memcpy(m_DrawFrameContext.LightAngleAttenCurveParams, &angleAttenCurve.u, sizeof(angleAttenCurve.u));
				}
			}
			else 
			{
				assert(0);
			}
		}
		else 
		{
			// use default build light (Point)
			m_DrawFrameContext.LightPosition = {-1.0f, -5.0f, 0.0f, 1.0f};
			m_DrawFrameContext.LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
			m_DrawFrameContext.LightDirection = {0.0f, 0.0f, -1.0f};
			m_DrawFrameContext.LightIntensity = 1.0f;
			m_DrawFrameContext.LightDistAttenCurveType = AttenCurveType::kAttenLinear;
			m_DrawFrameContext.LightDistAttenCurveParams[0] = 0.0f;
			m_DrawFrameContext.LightDistAttenCurveParams[1] = 1.0f;
			m_DrawFrameContext.LightAngleAttenCurveType = AttenCurveType::kAttenLinear;
			m_DrawFrameContext.LightAngleAttenCurveParams[0] = PI;
			m_DrawFrameContext.LightAngleAttenCurveParams[1] = PI;
		}
	}

	void GraphicsManager::InitializeBuffers(const Scene& scene)
	{
		std::cout << "[GraphicsManager] GraphicsManager::InitializeBuffers()" << std::endl;
	}
	
	void GraphicsManager::ClearBuffers()
	{
		std::cout << "[GraphicsManager] GraphicsManager::ClearBuffers()" << std::endl;
	}

	void GraphicsManager::RenderBuffers()
	{
		std::cout << "[RHI] GraphcisManager::RenderBuffers()" << std::endl;
	}

	#ifdef DEBUG
	void GraphicsManager::ClearDebugBuffers()
	{
		std::cout << "[GraphicsManager] GraphicsManager::ClearDebugBuffers(void)" << std::endl;
	}

	void GraphicsManager::DrawLine(const Point& from, const Point& to, const Vector3Df& color)
	{
		std::cout << "[GraphicsManager] GraphicsManager::DrawLine(" << from << ", "
			<< to << ", "
			<< color << ")" << std::endl;
	}

	void GraphicsManager::DrawLine(const PointList& vertices, const Vector3Df& color)
	{
		std::cout << "[GraphicsManager] GraphicsManager::DrawLine(" << vertices.size() << ","
			<< color << ")" << std::endl;
	}

	void GraphicsManager::DrawLine(const PointList& vertices, const Matrix4f& trans, const Vector3Df& color)
	{
		std::cout << "[GraphicsManager] GraphicsManager::DrawLine(" << vertices.size() << ","
			<< trans << "," 
			<< color << ")" << std::endl;
	}

	#endif
}

#include <iostream>
#include "GraphicsManager.hpp"
#include "SceneManager.hpp"
#include "Interface/IApplication.hpp"

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
		m_DrawFrameContext.AmbientColor = {0.2f, 0.2f, 0.2f};
		m_DrawFrameContext.Lights.clear();
		
		auto& scene = g_pSceneManager->GetScene();
		auto _pLightNode = scene.GetFirstLightNode();
		if (!_pLightNode) 
		{
			LightContext light;
			// use default build light (Point)
			light.LightPosition = {-1.0f, -5.0f, 0.0f, 1.0f};
			light.LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
			light.LightDirection = {0.0f, 0.0f, -1.0f, 0.0f};
			light.LightSize = { 0.0f, 0.0f };
			light.LightIntensity = 1.0f;
			light.LightDistAttenCurveType = AttenCurveType::kAttenLinear;
			light.LightDistAttenCurveParams[0] = 0.0f;
			light.LightDistAttenCurveParams[1] = 1.0f;
			light.LightAngleAttenCurveType = AttenCurveType::kAttenLinear;
			light.LightAngleAttenCurveParams[0] = PI;
			light.LightAngleAttenCurveParams[1] = PI;

			m_DrawFrameContext.Lights.push_back(light);

			return;
		}

		for (auto pLightNode : scene.LightNodes)
		{
			LightContext light;
			const std::shared_ptr<Matrix4f> transPtr = pLightNode.second->GetCalculatedTransform();
			light.LightPosition = {0.0f, 0.0f, 0.0f, 1.0f};
			TransformCoord(light.LightPosition, *transPtr);
			light.LightDirection = {0.0f, 0.0f, -1.0f, 0.0f};
			TransformCoord(light.LightDirection, *transPtr);

			const std::shared_ptr<SceneObjectLight> pLight = scene.GetLight(pLightNode.second->GetSceneObjectRef());
			if (pLight)
			{
				light.LightColor = pLight->GetColor().Value;
				light.LightIntensity = pLight->GetIntensity();
				const AttenCurve& attenCurve = pLight->GetDistanceAttenuation();
				light.LightDistAttenCurveType = attenCurve.type;
				memcpy(light.LightDistAttenCurveParams, &attenCurve.u, sizeof(attenCurve.u));
				if (pLight->GetType() == SceneObjectType::kSceneObjectTypeLightSpot)
				{
					const std::shared_ptr<SceneObjectSpotLight> _pLight = std::dynamic_pointer_cast<SceneObjectSpotLight>(pLight);
					const AttenCurve& angleAttenCurve = _pLight->GetAngleAttenuation();
					light.LightAngleAttenCurveType = angleAttenCurve.type;
					memcpy(light.LightAngleAttenCurveParams, &angleAttenCurve.u, sizeof(angleAttenCurve.u));
				}
				else if (pLight->GetType() == SceneObjectType::kSceneObjectTypeLightInfinite)
				{
					light.LightPosition.data[3] = 0.0f;
				}
				else if (pLight->GetType() == SceneObjectType::kSceneObjectTypeLightArea)
				{
					auto plight = std::dynamic_pointer_cast<SceneObjectAreaLight>(pLight);
					light.LightSize = plight->GetDimension();
				}
			}
			else 
			{
				assert(0);
			}

			m_DrawFrameContext.Lights.push_back(light);

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

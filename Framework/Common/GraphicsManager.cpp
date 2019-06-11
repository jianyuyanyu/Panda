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

/*
	bool GraphicsManager::SetPerFrameShaderParameters()
	{
		std::cout << "[RHI] GraphicsManager::SetPerFrameShaderParameters(void)" << std::endl;
		return true;
	}

	bool GraphicsManager::SetPerBatchShaderParameters(const char* paramName, const Matrix4f& param)
	{
		std::cout << "[RHI] GraphicsManager::SetPerFrameShaderParameters(const char* paramName, const Matrix4f& param)" << std::endl;
		std::cout << "paramName = " << paramName << std::endl;
		std::cout << "param = " << param << std::endl;
		return true;
	}

	bool GraphicsManager::SetPerBatchShaderParameters(const char* paramName, const Vector3Df& param)
	{
		std::cout << "[RHI] GraphicsManager::SetPerFrameShaderParameters(const char* paramName, const Vector3f& param)" << std::endl;
		std::cout << "paramName = " << paramName << std::endl;
		std::cout << "param = " << param << std::endl;
		return true;
	}

	bool GraphicsManager::SetPerBatchShaderParameters(const char* paramName, const float param)
	{
		std::cout << "[RHI] GraphicsManager::SetPerFrameShaderParameters(const char* paramName, const float param)" << std::endl;
		std::cout << "paramName = " << paramName << std::endl;
		std::cout << "param = " << param << std::endl;
		return true;
	}

	bool GraphicsManager::SetPerBatchShaderParameters(const char* paramName, const int param)
	{
		std::cout << "[RHI] GraphicsManager::SetPerFrameShaderParameters(const char* paramName, const int param)" << std::endl;
		std::cout << "paramName = " << paramName << std::endl;
		std::cout << "param = " << param << std::endl;
		return true;
	}
*/

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
			m_DrawFrameContext.ViewMatrix.SetInverse();
		}
		else 
		{
			// use default camera
			Vector3Df position = {0.0f, -5.0f, 0.0f}, lookAt = {0.0f, 0.0f, 0.0f}, up = {0.0f, 0.0f, 1.0f};
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
		auto& scene = g_pSceneManager->GetScene();
		auto pLightNode = scene.GetFirstLightNode();
		if (pLightNode)
		{
			m_DrawFrameContext.LightPosition = {0.0f, 0.0f, 0.0f};
			TransformCoord(m_DrawFrameContext.LightPosition, *pLightNode->GetCalculatedTransform());

			auto pLight = scene.GetLight(pLightNode->GetSceneObjectRef());
			if (pLight)
			{
				m_DrawFrameContext.LightColor = pLight->GetColor().Value;
			}
		}
		else 
		{
			// use default light
			m_DrawFrameContext.LightPosition = {-1.0f, -5.0f, -0.0f};
			m_DrawFrameContext.LightColor = {1.0f, 1.0f, 1.0f, 1.0f};
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

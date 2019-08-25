#include "DAE.hpp"
#include "assimp/Importer.hpp"
#include "Importer.h"
#include "assimp/scene.h"

namespace Panda
{
	void DaeParser::ConvertDaeStructureToSceneNode(const Assimp::ColladaParser& parser, const Assimp::Collada::Node& inNode, std::shared_ptr<BaseSceneNode>& baseNode, Scene& scene)
	{
		std::shared_ptr<BaseSceneNode> node;
		
		// mesh
		if (inNode.mMeshes.size() > 0)
		{
			std::string nodeId = inNode.mID;
			std::string nodeName = inNode.mName;
			auto _node = std::make_shared<SceneGeometryNode>(nodeName, nodeId);
			_node->SetVisibility(true);
			_node->SetIfCastShadow(true);
			_node->SetIfMotionBlur(false);

			// ref scene objects
			std::string meshId = inNode.mMeshes[0].mMeshOrController;
			_node->AddSceneObjectRef(meshId);

			// ref materials 
			for (auto mat : inNode.mMeshes[0].mMaterials)
			{
				_node->AddMaterialRef(mat.first);
			}

			scene.GeometryNodes.emplace(nodeId, _node);
			scene.LUTNameGeometryNode.emplace(nodeId, _node);
			
			node = _node;

			AppendTransform(node, inNode.mTransforms[0]);

			AddGeometryObject(scene, inNode.mMeshes[0],
				parser.mMeshLibrary, parser.mMaterialLibrary, 
				parser.mEffectLibrary);
		}
		else if (inNode.mCameras.size() > 0)
		{
			std::string nodeName = inNode.mName;

			auto _node = std::make_shared<SceneCameraNode>(nodeName);
			std::string id = inNode.mCameras[0].mCamera;
			auto iter = parser.mCameraLibrary.find(id);
			_node->AddSceneObjectRef(iter->first);

			scene.CameraNodes.emplace(iter->first, _node);

			node = _node;

			AppendTransform(node, inNode.mTransforms[0]);

			AddCameraObject(scene, iter->first, iter->second);
		}
		else if (inNode.mLights.size() > 0)
		{
			std::string nodeName = inNode.mName;

			auto _node = std::make_shared<SceneLightNode>(nodeName);
			std::string id = inNode.mLights[0].mLight;
			auto iter = parser.mLightLibrary.find(id);

			_node->SetIfCastShadow(true);
			_node->AddSceneObjectRef(iter->first);

			scene.LightNodes.emplace(iter->first, _node);

			node = _node;

			AppendTransform(node, inNode.mTransforms[0]);

			AddLightObject(scene, iter->first, iter->second);
		}

		// Recursive add children.
		uint32_t childrenCount = inNode.mChildren.size();
		for (int32_t i = 0; i < childrenCount; ++i)
		{
			ConvertDaeStructureToSceneNode(parser, *inNode.mChildren[i], node, scene);
		}

		baseNode->AppendChild(std::move(node));
	}

	void DaeParser::AddGeometryObject(Scene& scene, const std::string& inGeometryName, const Assimp::Collada::Mesh& inMesh)
	{
		std::string objectName = inGeometryName;
		std::shared_ptr<SceneObjectGeometry> _object = std::make_shared<SceneObjectGeometry>();
		_object->SetVisibility(true);
		_object->SetIfCastShadow(true);
		_object->SetIfMotionBlur(false);

		std::shared_ptr<SceneObjectMesh> mesh(new SceneObjectMesh());

		// primitive type
		// Faces. Stored are only the number of vertices for each face.
		// 1 == point, 2 == line, 3 == triangle, 4+ == poly
		if (inMesh.mFaceSize[0] == 1)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePointList);
		else if (inMesh.mFaceSize[0] == 2)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeLineList);
		else if (inMesh.mFaceSize[0] == 3)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeTriList);
		else if (inMesh.mFaceSize[0] == 4)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePolygon);
		else
			// not supported
			mesh.reset();

		// vertex data
		VertexDataType vertexDataType = VertexDataType::kVertexDataTypeFloat3;
		uint32_t elementCount = inMesh.mPositions.size();
		void* data = new float[elementCount * 3];
		size_t buffSize = sizeof(float) * elementCount * 3;
		const void* _data = &inMesh.mPositions[0];
		memcpy(data, _data, buffSize);
		SceneObjectVertexArray& _v_array = *new SceneObjectVertexArray("position", 0, vertexDataType, data, elementCount * 3);
		mesh->AddVertexArray(std::move(_v_array));

		// normal data
		void* pNormData = new float[elementCount * 3];
		memcpy(pNormData, (void*)(&inMesh.mNormals[0]), buffSize);
		SceneObjectVertexArray& _n_array = *new SceneObjectVertexArray("normal", 0, vertexDataType, pNormData, elementCount * 3);
		mesh->AddVertexArray(std::move(_n_array));

		// index data
		elementCount = inMesh.mFacePosIndices.size();
		_data = &inMesh.mFacePosIndices[0];
		int32_t data_size = 4;
		buffSize = data_size * elementCount;
		void* pIndexData = new uint8_t[buffSize];
		memcpy(pIndexData, _data, buffSize);
		// fix index bug. I can't do it in assimp because the indices relates to animation.
		uint32_t* viewData = (uint32_t*)pIndexData;
		for (int32_t i = 0; i < elementCount; ++i)
		{
			viewData[i] = i;
		}
		SceneObjectIndexArray& _i_array = *new SceneObjectIndexArray(0, 0, IndexDataType::kIndexDataTypeInt32, pIndexData, elementCount);
		mesh->AddIndexArray(std::move(_i_array));

		_object->AddMesh(mesh);

		scene.Geometries[objectName] = _object;
	}

	void DaeParser::AddGeometryObject(Scene& scene, const std::string& inGeometryName, 
		const Assimp::Collada::Mesh& inMesh, const Assimp::Collada::MeshInstance& meshInstance,
		const Assimp::ColladaParser::MaterialLibrary& inMaterialLib, const Assimp::ColladaParser::EffectLibrary& inEffectLib)
	{
		std::string objectName = inGeometryName;
		std::shared_ptr<SceneObjectGeometry> _object = std::make_shared<SceneObjectGeometry>();
		_object->SetVisibility(true);
		_object->SetIfCastShadow(true);
		_object->SetIfMotionBlur(false);

		std::shared_ptr<SceneObjectMesh> mesh(new SceneObjectMesh());

		// primitive type
		// Faces. Stored are only the number of vertices for each face.
		// 1 == point, 2 == line, 3 == triangle, 4+ == poly
		if (inMesh.mFaceSize[0] == 1)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePointList);
		else if (inMesh.mFaceSize[0] == 2)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeLineList);
		else if (inMesh.mFaceSize[0] == 3)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeTriList);
		else if (inMesh.mFaceSize[0] == 4)
			mesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePolygon);
		else
			// not supported
			mesh.reset();

		// vertex data
		VertexDataType vertexDataType = VertexDataType::kVertexDataTypeFloat3;
		uint32_t elementCount = inMesh.mPositions.size();
		void* data = new float[elementCount * 3];
		size_t buffSize = sizeof(float) * elementCount * 3;
		const void* _data = &inMesh.mPositions[0];
		memcpy(data, _data, buffSize);
		SceneObjectVertexArray& _v_array = *new SceneObjectVertexArray("position", 0, vertexDataType, data, elementCount * 3);
		mesh->AddVertexArray(std::move(_v_array));

		// normal data
		void* pNormData = new float[elementCount * 3];
		memcpy(pNormData, (void*)(&inMesh.mNormals[0]), buffSize);
		SceneObjectVertexArray& _n_array = *new SceneObjectVertexArray("normal", 0, vertexDataType, pNormData, elementCount * 3);
		mesh->AddVertexArray(std::move(_n_array));


		// index data which is conbined with material
		int matIndex = 0;
		for (auto mat : meshInstance.mMaterials)
		{
			elementCount = inMesh.mFacePosIndices.size();
			_data = &inMesh.mFacePosIndices[0];
			int32_t data_size = 4;
			buffSize = data_size * elementCount;
			void* pIndexData = new uint8_t[buffSize];
			memcpy(pIndexData, _data, buffSize);
			// fix index bug. I can't do it in assimp because the indices relates to animation.
			uint32_t* viewData = (uint32_t*)pIndexData;
			for (int32_t i = 0; i < elementCount; ++i)
			{
				viewData[i] = i;
			}
			SceneObjectIndexArray& _i_array = *new SceneObjectIndexArray(matIndex, 0, IndexDataType::kIndexDataTypeInt32, pIndexData, elementCount);
			mesh->AddIndexArray(std::move(_i_array));

			auto matIter = inMaterialLib.find(mat.first);
			auto effIter = inEffectLib.find(matIter->second.mEffect);
			AddMaterial(scene, mat.first, matIter->second.mEffect, effIter->second);

			++matIndex;
		}

		_object->AddMesh(mesh);

		scene.Geometries[objectName] = _object;
	}

	void DaeParser::AddGeometryObject(Scene& scene, const Assimp::Collada::MeshInstance& meshInstance,
		const Assimp::ColladaParser::MeshLibrary& inMeshLib, const Assimp::ColladaParser::MaterialLibrary& inMaterialLib, 
		const Assimp::ColladaParser::EffectLibrary& inEffectLib)
	{
		std::string meshId = meshInstance.mMeshOrController;
		auto iter = inMeshLib.find(meshId);
		const Assimp::Collada::Mesh& mesh = *iter->second;

		std::shared_ptr<SceneObjectGeometry> _object = std::make_shared<SceneObjectGeometry>();
		_object->SetVisibility(true);
		_object->SetIfCastShadow(true);
		_object->SetIfMotionBlur(false);
		
		if (mesh.mSubMeshes.size() > 0)
		{
			/// If we have submeshes, we must divide the object to parts of submeshes.
			for (int32_t i = 0; i < mesh.mSubMeshes.size(); ++i)
			{
				std::shared_ptr<SceneObjectMesh> ourMesh(new SceneObjectMesh());

				const Assimp::Collada::SubMesh& subMesh = mesh.mSubMeshes[i];
				
				uint32_t startFaceIndex = 0;
				uint32_t startVertexAndNormalIndex = 0;
				for (int32_t j = 0; j < i; ++j)
				{
					startVertexAndNormalIndex += mesh.mFaceSize[startFaceIndex] * mesh.mSubMeshes[j].mNumFaces;
					startFaceIndex += mesh.mSubMeshes[j].mNumFaces;
				}
				uint32_t faceCount = subMesh.mNumFaces;

				// primitive type
				// Faces. Stored are only the number of vertices for each face.
				// 1 == point, 2 == line, 3 == triangle, 4+ == poly
				if (mesh.mFaceSize[startFaceIndex] == 1)
					ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePointList);
				else if (mesh.mFaceSize[startFaceIndex] == 2)
					ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeLineList);
				else if (mesh.mFaceSize[startFaceIndex] == 3)
					ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeTriList);
				else if (mesh.mFaceSize[startFaceIndex] == 4)
					ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePolygon);
				else
					// not supported
					ourMesh.reset();

				// vertex data
				VertexDataType vertexDataType = VertexDataType::kVertexDataTypeFloat3;
				uint32_t elementCount = mesh.mFaceSize[startFaceIndex] * faceCount;
				void* data = new float[elementCount * 3];
				size_t buffSize = sizeof(float) * elementCount * 3;
				const void* _data = &mesh.mPositions[startVertexAndNormalIndex];
				memcpy(data, _data, buffSize);
				SceneObjectVertexArray& _v_array = *new SceneObjectVertexArray("position", 0, vertexDataType, data, elementCount * 3);
				ourMesh->AddVertexArray(std::move(_v_array));

				// normal data
				void* pNormData = new float[elementCount * 3];
				memcpy(pNormData, (void*)(&mesh.mNormals[startVertexAndNormalIndex]), buffSize);
				SceneObjectVertexArray& _n_array = *new SceneObjectVertexArray("normal", 0, vertexDataType, pNormData, elementCount * 3);
				ourMesh->AddVertexArray(std::move(_n_array));

				// index data
				elementCount = mesh.mFaceSize[startFaceIndex] * faceCount;
				int32_t dataSize = 4;
				buffSize = elementCount * dataSize;
				void* pIndexData = new uint8_t[buffSize];
				uint32_t* viewData = (uint32_t*)pIndexData;
				for (int32_t j = 0; j < elementCount; ++j)
				{
					viewData[j] = j;
				}
				SceneObjectIndexArray& _i_array = *new SceneObjectIndexArray(i, 0, IndexDataType::kIndexDataTypeInt32, pIndexData, elementCount);
				ourMesh->AddIndexArray(std::move(_i_array));

				
				auto matIter = inMaterialLib.find(subMesh.mMaterial);
				if (matIter != inMaterialLib.end())
				{
					auto effIter = inEffectLib.find(matIter->second.mEffect);
					AddMaterial(scene, subMesh.mMaterial, matIter->second.mEffect, effIter->second);
				}
				else
				{
					AddDefaultMaterial(scene);
				}

				_object->AddMesh(ourMesh);
			}
		}
		else
		{
			/// No submeshes. It means we only have one mesh.

			std::shared_ptr<SceneObjectMesh> ourMesh(new SceneObjectMesh());
			// primitive type
			// Faces. Stored are only the number of vertices for each face.
			// 1 == point, 2 == line, 3 == triangle, 4+ == poly
			if (mesh.mFaceSize[0] == 1)
				ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePointList);
			else if (mesh.mFaceSize[0] == 2)
				ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeLineList);
			else if (mesh.mFaceSize[0] == 3)
				ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypeTriList);
			else if (mesh.mFaceSize[0] == 4)
				ourMesh->SetPrimitiveType(PrimitiveType::kPrimitiveTypePolygon);
			else
				// not supported
				ourMesh.reset();

			// vertex data
			VertexDataType vertexDataType = VertexDataType::kVertexDataTypeFloat3;
			uint32_t elementCount = mesh.mPositions.size();
			void* data = new float[elementCount * 3];
			size_t buffSize = sizeof(float) * elementCount * 3;
			const void* _data = &mesh.mPositions[0];
			memcpy(data, _data, buffSize);
			SceneObjectVertexArray& _v_array = *new SceneObjectVertexArray("position", 0, vertexDataType, data, elementCount * 3);
			ourMesh->AddVertexArray(std::move(_v_array));

			// normal data
			void* pNormData = new float[elementCount * 3];
			memcpy(pNormData, (void*)(&mesh.mNormals[0]), buffSize);
			SceneObjectVertexArray& _n_array = *new SceneObjectVertexArray("normal", 0, vertexDataType, pNormData, elementCount * 3);
			ourMesh->AddVertexArray(std::move(_n_array));

			elementCount = mesh.mFacePosIndices.size();
			_data = &mesh.mFacePosIndices[0];
			int32_t data_size = 4;
			buffSize = data_size * elementCount;
			void* pIndexData = new uint8_t[buffSize];
			memcpy(pIndexData, _data, buffSize);
			// fix index bug. I can't do it in assimp because the indices relates to animation.
			uint32_t* viewData = (uint32_t*)pIndexData;
			for (int32_t i = 0; i < elementCount; ++i)
			{
				viewData[i] = i;
			}
			SceneObjectIndexArray& _i_array = *new SceneObjectIndexArray(0, 0, IndexDataType::kIndexDataTypeInt32, pIndexData, elementCount);
			ourMesh->AddIndexArray(std::move(_i_array));

			auto matIter = inMaterialLib.find(meshInstance.mMaterials.begin()->first);
			auto effIter = inEffectLib.find(matIter->second.mEffect);
			AddMaterial(scene, meshInstance.mMaterials.begin()->first, matIter->second.mEffect, effIter->second);

			_object->AddMesh(ourMesh);
		}

		scene.Geometries[meshId] = _object;
	}

	void DaeParser::AppendTransform(std::shared_ptr<BaseSceneNode>& baseNode, const Assimp::Collada::Transform& transform)
	{
		Matrix4f matrix;
		std::shared_ptr<SceneObjectTransform> _transform;

		matrix = transform.f;
		TransposeMatrix(matrix, matrix);
		_transform = std::make_shared<SceneObjectTransform>(matrix, false);
		baseNode->AppendTransform("", std::move(_transform));
	}

	void DaeParser::AddLightObject(Scene& scene, const std::string& inLightName, const Assimp::Collada::Light& inLight)
	{
		std::string lightName = inLightName;
		std::shared_ptr<SceneObjectLight> light;
		switch (inLight.mType)
		{
		case aiLightSourceType::aiLightSource_POINT:
		{
			light = std::make_shared<SceneObjectPointLight>();
			break;
		}
		case aiLightSourceType::aiLightSource_DIRECTIONAL:
		{
			light = std::make_shared<SceneObjectInfiniteLight>();
			break;
		}
		case aiLightSourceType::aiLightSource_SPOT:
		{
			light = std::make_shared<SceneObjectSpotLight>();
			break;
		}
		case aiLightSourceType::aiLightSource_AREA:
		{
			light = std::make_shared<SceneObjectAreaLight>();
			break;
		}
		default:
			assert(0);
			break;
		}
		
		light->SetIfCastShadow(true);
		
		std::string attrib;
		// color
		
		attrib = "light";
		// I use blender 2.8.0. The default value is 1000. So, just divide by 1000.
		Vector4Df color({ inLight.mColor.r / 1000.0f, inLight.mColor.g / 1000.0f, inLight.mColor.b / 1000.0f, 1.0f });
		light->SetColor(attrib, color);

		// atten
		AttenCurve curve;
		curve.type = AttenCurveType::kAttenInverseSquare;
		curve.u.inverseSquareParams.scale = 1.0f;
		curve.u.inverseSquareParams.offset = 0.0f;
		curve.u.inverseSquareParams.kc = inLight.mAttConstant;
		curve.u.inverseSquareParams.kl = inLight.mAttLinear;
		curve.u.inverseSquareParams.kq = inLight.mAttQuadratic;

		light->SetDistanceAttenuation(curve);

		scene.Lights[lightName] = light;
	}

	void DaeParser::AddCameraObject(Scene& scene, const std::string& inCameraName, const Assimp::Collada::Camera& inCamera)
	{
		std::string cameraName = inCameraName;
		auto camera = std::make_shared<SceneObjectPerspectiveCamera>();

		// Set fov
		std::string param = "fov";
		float fov = inCamera.mHorFov / 180.f * PI;
		camera->SetParam(param, fov);

		// Set z-near
		param = "near";
		camera->SetParam(param, inCamera.mZNear);

		// Set z-far
		param = "far";
		camera->SetParam(param, inCamera.mZFar);
		
		// Set aspect ratio
		param = "aspect";
		camera->SetParam(param, inCamera.mAspect);

		scene.Cameras[cameraName] = camera;
	}

	void DaeParser::AddMaterial(Scene& scene, const std::string& keyName, const std::string& matName, const Assimp::Collada::Effect& effect)
	{
		auto material = std::make_shared<SceneObjectMaterial>();
		material->SetName(matName);

		std::string attrib;
		Vector4Df color;
		attrib = "diffuse";
		color = { effect.mDiffuse.r, effect.mDiffuse.g, effect.mDiffuse.b, effect.mDiffuse.a };
		material->SetColor(attrib, color);

		attrib = "specular";
		color = { effect.mSpecular.r, effect.mSpecular.g, effect.mSpecular.b, effect.mSpecular.a };
		material->SetColor(attrib, color);

		attrib = "emission";
		color = { effect.mEmissive.r, effect.mEmissive.g, effect.mEmissive.b, effect.mEmissive.a };
		material->SetColor(attrib, color);

		attrib = "transparency";
		color = { effect.mTransparent.r, effect.mTransparent.g, effect.mTransparent.b, effect.mTransparent.a };
		material->SetColor(attrib, color);

		attrib = "specular_power";
		material->SetParam(attrib, effect.mShininess);

		scene.Materials[keyName] = material;
	}

	void DaeParser::AddDefaultMaterial(Scene& scene)
	{
		auto material = std::make_shared<SceneObjectMaterial>();
		material->SetName("default");

		scene.Materials["default"] = material;
	}

	const aiLight* DaeParser::FindaiLight(const aiScene& _aiScene, std::string lightName)
	{
		uint32_t lightNum = _aiScene.mNumLights;
		for (uint32_t i = 0; i < lightNum; ++i)
		{
			std::string cLightName = _aiScene.mLights[i]->mName.C_Str();
			if (cLightName == lightName)
				return _aiScene.mLights[i];
		}

		return nullptr;
	}

	const aiCamera* DaeParser::FindaiCamera(const aiScene& _aiScene, std::string cameraName)
	{
		uint32_t cameraNum = _aiScene.mNumCameras;

		for (uint32_t i = 0; i < cameraNum; ++i)
		{
			std::string cCameraName = _aiScene.mCameras[i]->mName.C_Str();
			if (cCameraName == cameraName)
				return _aiScene.mCameras[i];
		}

		return nullptr;
	}
	
    std::unique_ptr<Scene> DaeParser::Parse(const std::string& file)
    {
        Assimp::Importer importer;
		Assimp::ImporterPimpl* pimpl = importer.Pimpl();
		Assimp::ColladaParser parser(pimpl->mIOHandler, file);
		Assimp::Collada::Node* pRootNode = parser.mRootNode;

		std::unique_ptr<Scene> pScene(new Scene("Root"));
		uint32_t childrenCount = pRootNode->mChildren.size();
		for (int32_t i = 0; i < childrenCount; ++i)
		{
			ConvertDaeStructureToSceneNode(parser, *pRootNode->mChildren[i], pScene->SceneGraph, *pScene);
		}
		
		return pScene;
    }
}
#include "GraphicsManager.hpp"

using namespace Panda;

int Panda::GraphicsManager::Initialize()
{
	int result = 0;
	return result;
}

void Panda::GraphicsManager::Finalize()
{
}

void Panda::GraphicsManager::Tick()
{
}

void Panda::GraphicsManager::DrawSingleMesh(const Mesh& mesh)
{
	std::cout << "DrawSingleMesh" << std::endl;
	std::cout << "--------------" << std::endl;
	std::cout << "Vertex Buffer: " 			<< mesh.m_VertexBuffer << std::endl;
	std::cout << "Vertex Buffer Size: "		<< mesh.m_VertexBufferSize << std::endl;
	std::cout << "Vertex Count: "			<< mesh.m_VertexCount << std::endl;
	std::cout << "Vertex Attribute Count: " << mesh.m_VertexAttributeCount << std::endl;
	std::cout << std::endl;
	std::cout << "Index Buffer: "			<< mesh.m_IndexBuffer << std::endl;
	std::cout << "Index Buffer Size: " 		<< mesh.m_IndexBufferSize << std::endl;
	std::cout << "Index Count: "			<< mesh.m_IndexCount << std::endl;
	std::cout << "Index Type: " 			<< mesh.m_IndexType << std::endl;
}


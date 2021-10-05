#include "DynamicVerticesProvider.h"
#include "DynamicVertices.h"
#include "Vertex.h"

DynamicVerticesProvider::DynamicVerticesProvider() = default;
DynamicVerticesProvider::~DynamicVerticesProvider() = default;

void DynamicVerticesProvider::addDynamicVertices(std::string name, std::unique_ptr<DynamicVertices> dynamicVertices)
{
	dynamicVerticesMap[name] = std::move(dynamicVertices);
}

std::vector<DynamicVertices*> DynamicVerticesProvider::getDynamicVerticesList() const
{
	std::vector<DynamicVertices*> dynamicVerticesList;
	for (auto& dynamicVertices : dynamicVerticesMap)
	{
		dynamicVerticesList.push_back(dynamicVertices.second.get());
	}
	return dynamicVerticesList;
}

std::vector<UINT> DynamicVerticesProvider::getVertexBufferSizes()
{
	std::vector<UINT> vertexBufferSizes;
	for (auto& dynamicVertices : dynamicVerticesMap)
	{
		size_t size = dynamicVertices.second.get()->getVertices().size();
		vertexBufferSizes.push_back((UINT)size);
	}
	return vertexBufferSizes;
}

DynamicVertices* DynamicVerticesProvider::getDynamicVerticesForName(std::string name) const
{
	return dynamicVerticesMap.find(name)->second.get();
}

void DynamicVerticesProvider::update(const GameTimer& gameTimer)
{
	for (auto& dynamicVertices : dynamicVerticesMap)
	{
		dynamicVertices.second->update(gameTimer);
	}
}
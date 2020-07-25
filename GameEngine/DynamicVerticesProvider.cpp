#include "DynamicVerticesProvider.h"
#include "DynamicVertices.h"
#include "Waves.h"

DynamicVerticesProvider::DynamicVerticesProvider()
{
	dynamicVerticesMap["waves"] = waves();
}

DynamicVerticesProvider::~DynamicVerticesProvider() = default;

std::unique_ptr<Waves> DynamicVerticesProvider::waves()
{
	auto waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
	return waves;
}

std::vector <DynamicVertices*> DynamicVerticesProvider::getDynamicVerticesList()
{
	std::vector<DynamicVertices*> dynamicVerticesList;
	for (auto& dynamicVertices : dynamicVerticesMap)
	{
		dynamicVerticesList.push_back(dynamicVertices.second.get());
	}
	return dynamicVerticesList;
}

DynamicVertices* DynamicVerticesProvider::getDynamicVerticesForName(std::string name)
{
	return dynamicVerticesMap[name].get();
}

void DynamicVerticesProvider::update(const GameTimer& gameTimer)
{
	for (auto& dynamicVertices : dynamicVerticesMap)
	{
		dynamicVertices.second->update(gameTimer);
	}
}
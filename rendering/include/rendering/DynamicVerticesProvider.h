#pragma once

#include <unordered_map>
#include <memory>
#include <Windows.h>

class DynamicVertices;
class GameTimer;

class DynamicVerticesProvider
{
public:
	DynamicVerticesProvider();
	~DynamicVerticesProvider();

	std::vector<UINT> getVertexBufferSizes();
	std::vector <DynamicVertices*> getDynamicVerticesList() const;
	DynamicVertices* getDynamicVerticesForName(std::string name) const;
	void addDynamicVertices(std::string name, std::unique_ptr<DynamicVertices> dynamicVertices);
	void update(const GameTimer& gameTimer);

private:
	std::unordered_map<std::string, std::unique_ptr<DynamicVertices>> dynamicVerticesMap;
};


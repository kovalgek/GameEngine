#include <unordered_map>
#include <memory>

class DynamicVertices;
class Waves;
class GameTimer;

#pragma once
class DynamicVerticesProvider
{
public:
	DynamicVerticesProvider();
	~DynamicVerticesProvider();
	std::vector <DynamicVertices*> getDynamicVerticesList();
	DynamicVertices* getDynamicVerticesForName(std::string name);
	void update(const GameTimer& gameTimer);
private:
	std::unique_ptr<Waves> waves();
	std::unordered_map<std::string, std::unique_ptr<DynamicVertices>> dynamicVerticesMap;
};


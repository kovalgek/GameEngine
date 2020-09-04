#include <vector>

class GameTimer;
struct Vertex;

#pragma once
class DynamicVertices
{
public:
	virtual void update(const GameTimer& gameTimer) = 0;
	virtual std::vector<Vertex> getVertices() = 0;
};



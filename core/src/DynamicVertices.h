#include <vector>
#include "UploadBuffer.h"

class GameTimer;
struct Vertex;

#pragma once
class DynamicVertices
{
public:
	virtual void update(const GameTimer& gameTimer) = 0;
	virtual std::vector<Vertex> getVertices() = 0;
	virtual std::vector<std::uint16_t> getIndices() = 0;

	UploadBuffer<Vertex>* vertexBuffer;
};



#pragma once

#include <memory>
#include <vector>
#include "ConstantBufferUpdating.h"

class GPUService;
class ConstantBufferUpdating;
struct FrameResource;
class GameTimer;

class FrameResourceUpdater
{	
public:
	FrameResourceUpdater(GPUService& gpuService, std::vector<std::unique_ptr<ConstantBufferUpdating>> constantBufferUpdaters);
	~FrameResourceUpdater() = default;

	void update(FrameResource& frameResource, const GameTimer& gameTimer);

private:
	std::vector<std::unique_ptr<ConstantBufferUpdating>> constantBufferUpdaters;
	GPUService& gpuService;

	void waitForFrameResourceAvailable(FrameResource& frameResource);
};


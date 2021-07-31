#include "FrameResourceUpdater.h"
#include "GPUService.h"
#include "ConstantBufferUpdating.h"
#include "FrameResource.h"
#include "GameTimer.h"

FrameResourceUpdater::FrameResourceUpdater(
	GPUService& gpuService, 
	std::vector<std::unique_ptr<ConstantBufferUpdating>> constantBufferUpdaters
) :
	gpuService { gpuService },
	constantBufferUpdaters{ std::move(constantBufferUpdaters) }
{

}

void FrameResourceUpdater::update(FrameResource & frameResource, const GameTimer& gameTimer)
{
	waitForFrameResourceAvailable(frameResource);

	for (auto& constantBufferUpdater : constantBufferUpdaters)
	{
		constantBufferUpdater->update(frameResource, gameTimer);
	}
}

void FrameResourceUpdater::waitForFrameResourceAvailable(FrameResource& frameResource)
{
	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	gpuService.waitForGPUFence(frameResource.Fence);
}
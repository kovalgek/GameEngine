#include "MainPassData.h"
#include <DirectXMath.h>
#include "GameTimer.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "FrameResourceController.h"
#include <d3d12.h>
#include "RenderItem.h"

#pragma once
class FrameResourceUpdater
{
	
public:
	FrameResourceUpdater(
		std::shared_ptr<FrameResourceController> frameResourceController,
		ID3D12Fence *fence,
		std::shared_ptr<MainPassDataProvider> mainPassDataProvider,
		std::shared_ptr<ObjectsDataProvider> objectsDataProvider);
	void update(const GameTimer& gameTimer);

private:
	std::shared_ptr<FrameResourceController> frameResourceController;
	ID3D12Fence* fence;
	std::shared_ptr<MainPassDataProvider> mainPassDataProvider;
	std::shared_ptr<ObjectsDataProvider> objectsDataProvider;

	void waitForAvailableResource();
	void updateMainPassCB(const GameTimer& gameTimer, MainPassData mainPassData);
	void updateObjectCBs(std::vector<std::shared_ptr<RenderItem>> allRitems);
};


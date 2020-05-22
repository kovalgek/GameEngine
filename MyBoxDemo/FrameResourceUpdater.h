#include "MainPassData.h"
#include <memory>
#include <vector>

struct RenderItem;
class FrameResourceController;
class MainPassDataProvider;
class ObjectsDataProvider;
class GameTimer;
struct ID3D12Fence;

#pragma once
class FrameResourceUpdater
{
	
public:
	FrameResourceUpdater(
		std::shared_ptr<FrameResourceController> frameResourceController,
		ID3D12Fence *fence,
		std::shared_ptr<MainPassDataProvider> mainPassDataProvider,
		ObjectsDataProvider *objectsDataProvider
	);

	void update(const GameTimer& gameTimer);

private:
	std::shared_ptr<FrameResourceController> frameResourceController;
	ID3D12Fence* fence;
	std::shared_ptr<MainPassDataProvider> mainPassDataProvider;
	ObjectsDataProvider *objectsDataProvider;

	void waitForAvailableResource();
	void updateMainPassCB(const GameTimer& gameTimer, MainPassData mainPassData);
	void updateObjectCBs(std::vector<std::shared_ptr<RenderItem>> allRitems);
};


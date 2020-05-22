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
		std::unique_ptr<FrameResourceController> frameResourceController,
		ID3D12Fence *fence,
		MainPassDataProvider *mainPassDataProvider,
		ObjectsDataProvider *objectsDataProvider
	);
	~FrameResourceUpdater();

	void update(const GameTimer& gameTimer);

private:
	std::unique_ptr<FrameResourceController> frameResourceController;
	ID3D12Fence* fence;
	MainPassDataProvider *mainPassDataProvider;
	ObjectsDataProvider *objectsDataProvider;

	void waitForAvailableResource();
	void updateMainPassCB(const GameTimer& gameTimer, MainPassData mainPassData);
	void updateObjectCBs(std::vector<std::shared_ptr<RenderItem>> allRitems);
};


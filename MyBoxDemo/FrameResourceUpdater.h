#include "MainPassData.h"
#include <memory>
#include <vector>

struct RenderItem;
class FrameResourceController;
class MainPassDataProvider;
class ObjectsDataProvider;
class GameTimer;
struct ID3D12Fence;
class Waves;
struct Material;

#pragma once
class FrameResourceUpdater
{
	
public:
	FrameResourceUpdater(
		std::unique_ptr<FrameResourceController> frameResourceController,
		ID3D12Fence* fence,
		MainPassDataProvider* mainPassDataProvider,
		ObjectsDataProvider* objectsDataProvider,
		Waves* waves
	);
	~FrameResourceUpdater();

	void update(const GameTimer& gameTimer);

private:
	std::unique_ptr<FrameResourceController> frameResourceController;
	ID3D12Fence* fence;
	MainPassDataProvider *mainPassDataProvider;
	ObjectsDataProvider *objectsDataProvider;
	Waves* waves;

	void waitForAvailableResource();
	void updateMainPassCB(const GameTimer& gameTimer, MainPassData mainPassData);
	void updateObjectCBs(std::vector<RenderItem*> allRitems);
	void updateWaves(const GameTimer& gameTimer, RenderItem* wavesRitem);
	void updateMaterialCBs(const GameTimer& gt, std::vector<Material*> materials);
};


#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <memory>
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "FrameResource.h"
#include "RenderItem.h"

class Application;
class GameTimer;
class FrameResourceController;
class ObjectsDataProvider;
class PipleneStateData;

const int gNumFrameResources = 3;

#pragma once
class MainScene
{
public:
	MainScene(
		std::weak_ptr<Application> appContext,
		std::unique_ptr<PipleneStateData> pipleneStateData,
		std::shared_ptr<FrameResourceController> frameResourceController,
		std::shared_ptr<ObjectsDataProvider> objectsDataProvider);

	~MainScene();
	void draw(const GameTimer& gameTimer);

private:
	std::weak_ptr<Application> appContext;

	ID3D12Device              *device;
	ID3D12CommandQueue        *commandQueue;
	ID3D12CommandAllocator    *commandAllocator;
	ID3D12GraphicsCommandList *commandList;

	std::shared_ptr <FrameResourceController> frameResourceController;
	std::unique_ptr <PipleneStateData> pipleneStateData;
	std::shared_ptr<ObjectsDataProvider> objectsDataProvider;

	void onKeyboardInput(const GameTimer& gameTimer);
	void drawRenderItems(ID3D12GraphicsCommandList* cmdList);

	bool isWireframe = false;
};


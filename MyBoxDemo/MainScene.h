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
#include "GeometryStorage.h"

class Application;
class GameTimer;
class FrameResourceController;
class PipleneStateData;

const int gNumFrameResources = 3;

#pragma once
class MainScene
{
public:
	MainScene(
		std::weak_ptr<Application> appContext,
		std::unique_ptr <PipleneStateData> pipleneStateData,
		std::unique_ptr <GeometryStorage> geometryStorage,
		std::unique_ptr <FrameResourceController> frameResourceController,
		std::vector<std::unique_ptr<RenderItem>> allRitems);

	~MainScene();
	void update(const GameTimer& gameTimer);
	void draw(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int x, int y);

private:
	std::weak_ptr<Application> appContext;

	ID3D12Device              *device;
	ID3D12CommandQueue        *commandQueue;
	ID3D12CommandAllocator    *commandAllocator;
	ID3D12GraphicsCommandList *commandList;

	std::unique_ptr <FrameResourceController> frameResourceController;
	std::unique_ptr <PipleneStateData> pipleneStateData;
	std::unique_ptr <GeometryStorage> geometryStorage;

	void updateCamera(const GameTimer& gameTimer);
	void onKeyboardInput(const GameTimer& gameTimer);
	void updateObjectCBs(const GameTimer& gameTimer);
	void updateMainPassCB(const GameTimer& gameTimer);
	void drawRenderItems(ID3D12GraphicsCommandList* cmdList);// , const std::vector<RenderItem*>& ritems);



	
	DirectX::XMFLOAT3 eyePosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * DirectX::XM_PI;
	float mPhi = 0.2f * DirectX::XM_PI;
	float mRadius = 15.0f;

	POINT mLastMousePos;


	std::vector<std::unique_ptr<RenderItem>> allRitems;


	PassConstants mainPassCB;

	bool isWireframe = false;
};


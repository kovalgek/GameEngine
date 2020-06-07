#include "FrameResourceUpdater.h"
#include "PassConstants.h"
#include "FrameResource.h"
#include "UploadBuffer.h"
#include "FrameResourceController.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "RenderItem.h"
#include "GameTimer.h"
#include "Waves.h"
#include <DirectXColors.h>

using namespace DirectX;

FrameResourceUpdater::FrameResourceUpdater(
	std::unique_ptr<FrameResourceController> frameResourceController,
	ID3D12Fence* fence,
	MainPassDataProvider *mainPassDataProvider,
	ObjectsDataProvider *objectsDataProvider,
	Waves *waves) :

	frameResourceController { std::move(frameResourceController) },
	fence { fence },
	mainPassDataProvider { mainPassDataProvider },
	objectsDataProvider { objectsDataProvider },
	waves { waves }
{
}

FrameResourceUpdater::~FrameResourceUpdater() = default;

void FrameResourceUpdater::update(const GameTimer& gameTimer)
{
	frameResourceController->changeFrameResource();

	waitForAvailableResource();

	auto mainPassData = mainPassDataProvider->getMainPassData();
	updateMainPassCB(gameTimer, mainPassData);

	auto renderItems = objectsDataProvider->renderItems();
	updateObjectCBs(renderItems);

	auto wavesRitem = objectsDataProvider->getWavesRitem();
	updateWaves(gameTimer, wavesRitem);
}

void FrameResourceUpdater::waitForAvailableResource()
{
	auto currFrameResource = frameResourceController->getCurrentFrameResource();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (currFrameResource->Fence != 0 && fence->GetCompletedValue() < currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(fence->SetEventOnCompletion(currFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void FrameResourceUpdater::updateMainPassCB(const GameTimer& gameTimer, MainPassData mainPassData)
{
	XMMATRIX view = XMLoadFloat4x4(&mainPassData.mView);
	XMMATRIX proj = XMLoadFloat4x4(&mainPassData.mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	PassConstants mainPassCB;
	XMStoreFloat4x4(&mainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mainPassCB.EyePosW = mainPassData.eyePosition;
	mainPassCB.RenderTargetSize = XMFLOAT2((float)mainPassData.clientWidth, (float)mainPassData.clientHeight);
	mainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mainPassData.clientWidth, 1.0f / mainPassData.clientHeight);
	mainPassCB.NearZ = 1.0f;
	mainPassCB.FarZ = 1000.0f;
	mainPassCB.TotalTime = gameTimer.TotalTime();
	mainPassCB.DeltaTime = gameTimer.DeltaTime();

	auto currPassCB = frameResourceController->getCurrentFrameResource()->PassCB.get();
	currPassCB->CopyData(0, mainPassCB);
}

void FrameResourceUpdater::updateObjectCBs(std::vector<RenderItem*> allRitems)
{
	auto currObjectCB = frameResourceController->getCurrentFrameResource()->ObjectCB.get();
	for (auto& e : allRitems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void FrameResourceUpdater::updateWaves(const GameTimer& gameTimer, RenderItem* wavesRitem)
{
	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((gameTimer.TotalTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, waves->RowCount() - 5);
		int j = MathHelper::Rand(4, waves->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		waves->Disturb(i, j, r);
	}

	// Update the wave simulation.
	waves->Update(gameTimer.DeltaTime());

	// Update the wave vertex buffer with the new solution.
	auto currWavesVB = frameResourceController->getCurrentFrameResource()->WavesVB.get();
	for (int i = 0; i < waves->VertexCount(); ++i)
	{
		Vertex v;

		v.Pos = waves->Position(i);
		v.Color = XMFLOAT4(DirectX::Colors::Blue);

		currWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	wavesRitem->Geo->VertexBufferGPU = currWavesVB->Resource();
}


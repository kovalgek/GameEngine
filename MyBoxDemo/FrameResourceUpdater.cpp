#include "FrameResourceUpdater.h"
#include "PassConstants.h"
#include "FrameResource.h"
#include "UploadBuffer.h"
#include "FrameResourceController.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "RenderItem.h"
#include "GameTimer.h"

using namespace DirectX;

FrameResourceUpdater::FrameResourceUpdater(
	std::shared_ptr<FrameResourceController> frameResourceController,
	ID3D12Fence* fence,
	std::shared_ptr<MainPassDataProvider> mainPassDataProvider,
	ObjectsDataProvider *objectsDataProvider) :

	frameResourceController { frameResourceController },
	fence { fence },
	mainPassDataProvider { mainPassDataProvider },
	objectsDataProvider { objectsDataProvider }
{
}

void FrameResourceUpdater::update(const GameTimer& gameTimer)
{
	frameResourceController->changeFrameResource();

	waitForAvailableResource();

	auto mainPassData = mainPassDataProvider->getMainPassData();
	updateMainPassCB(gameTimer, mainPassData);

	auto renderItems = objectsDataProvider->renderItems();
	updateObjectCBs(renderItems);
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

void FrameResourceUpdater::updateObjectCBs(std::vector<std::shared_ptr<RenderItem>> allRitems)
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

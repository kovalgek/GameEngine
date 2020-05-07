#include "MainScene.h"
#include "Application.h"
#include "d3dx12.h"
#include "d3dUtil.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "GameTimer.h"
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <array>
#include "FrameResourceController.h"
#include "PipleneStateData.h"


using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


MainScene::MainScene(
	std::weak_ptr<Application> appContext,
	std::unique_ptr <PipleneStateData> pipleneStateData,
	std::unique_ptr <GeometryStorage> geometryStorage,
	std::unique_ptr <FrameResourceController> frameResourceController,
	std::vector<std::unique_ptr<RenderItem>> allRitems) :

	appContext{ appContext },
	pipleneStateData { std::move(pipleneStateData) },
	geometryStorage { std::move(geometryStorage) },
	frameResourceController { std::move(frameResourceController) },
	allRitems { std::move(allRitems) },

	device { appContext.lock()->getDevice() },
	commandQueue { appContext.lock()->getCommandQueue() },
	commandList { appContext.lock()->getCommandList() },
	commandAllocator { appContext.lock()->getCommandAllocator() }
{


	//// Wait until initialization is complete.
	appContext.lock()->flushCommandQueue();
}

MainScene::~MainScene()
{

}

void MainScene::onResize(int clientWidth, int clientHeight)
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	float aspectRatio = static_cast<float>(clientWidth) / clientHeight;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void MainScene::updateCamera(const GameTimer& gameTimer)
{
	// Convert Spherical to Cartesian coordinates.
	eyePosition.x = mRadius * sinf(mPhi) * cosf(mTheta);
	eyePosition.z = mRadius * sinf(mPhi) * sinf(mTheta);
	eyePosition.y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}


void MainScene::update(const GameTimer& gameTimer)
{
	onKeyboardInput(gameTimer);
	updateCamera(gameTimer);


	frameResourceController->changeFrameResource();
	auto currFrameResource = frameResourceController->getCurrentFrameResource();

	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (currFrameResource->Fence != 0 && appContext.lock()->getFence()->GetCompletedValue() < currFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(appContext.lock()->getFence()->SetEventOnCompletion(currFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	updateObjectCBs(gameTimer);
	updateMainPassCB(gameTimer);
}

void MainScene::updateObjectCBs(const GameTimer& gameTimer)
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

void MainScene::updateMainPassCB(const GameTimer& gameTimer)
{
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mainPassCB.EyePosW = eyePosition;
	mainPassCB.RenderTargetSize = XMFLOAT2((float)800, (float)600);
	mainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / appContext.lock()->getClientWidth(), 1.0f / appContext.lock()->getClientHeight());
	mainPassCB.NearZ = 1.0f;
	mainPassCB.FarZ = 1000.0f;
	mainPassCB.TotalTime = gameTimer.TotalTime();
	mainPassCB.DeltaTime = gameTimer.DeltaTime();

	auto currPassCB = frameResourceController->getCurrentFrameResource()->PassCB.get();
	currPassCB->CopyData(0, mainPassCB);
}

void MainScene::onKeyboardInput(const GameTimer& gameTimer)
{
	if (GetAsyncKeyState('1') & 0x8000)
		isWireframe = true;
	else
		isWireframe = false;
}

void MainScene::draw(const GameTimer& gameTimer)
{
	auto cmdListAlloc = frameResourceController->getCurrentFrameResource()->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	if (isWireframe)
	{
		ThrowIfFailed(commandList->Reset(cmdListAlloc.Get(), pipleneStateData->getPSO("opaque_wireframe")));
	}
	else
	{
		ThrowIfFailed(commandList->Reset(cmdListAlloc.Get(), pipleneStateData->getPSO("opaque")));
	}

	commandList->RSSetViewports(1, &appContext.lock()->getScreenViewport());
	commandList->RSSetScissorRects(1, &appContext.lock()->getScissorRect());

	// Indicate a state transition on the resource usage.
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(appContext.lock()->currentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	commandList->ClearRenderTargetView(appContext.lock()->currentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	commandList->ClearDepthStencilView(appContext.lock()->depthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	commandList->OMSetRenderTargets(1, &appContext.lock()->currentBackBufferView(), true, &appContext.lock()->depthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { frameResourceController->getCbvHeap() };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetGraphicsRootSignature(pipleneStateData->getRootSignature());

	commandList->SetGraphicsRootDescriptorTable(1, frameResourceController->passCbvHandle());

	drawRenderItems(commandList);

	// Indicate a state transition on the resource usage.
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(appContext.lock()->currentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(commandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	appContext.lock()->swapChainMethod();

	// Advance the fence value to mark commands up to this fence point.
	frameResourceController->getCurrentFrameResource()->Fence = ++appContext.lock()->currentFence;

	// Add an instruction to the command queue to set a new fence point. 
	// Because we are on the GPU timeline, the new fence point won't be 
	// set until the GPU finishes processing all the commands prior to this Signal().
	commandQueue->Signal(appContext.lock()->getFence(), appContext.lock()->currentFence);
}

void MainScene::drawRenderItems(ID3D12GraphicsCommandList* cmdList)//, std::vector<std::unique_ptr<RenderItem>>& ritems)
{
	UINT objCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = frameResourceController->getCurrentFrameResource()->ObjectCB->Resource();

	// For each render item...
	for (size_t i = 0; i < allRitems.size(); ++i)
	{
		auto ri = allRitems[i].get();

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		// Offset to the CBV in the descriptor heap for this object and for this frame resource.
		cmdList->SetGraphicsRootDescriptorTable(0, frameResourceController->cbvHandle(ri->ObjCBIndex));

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}


void MainScene::onMouseDown(int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MainScene::onMouseUp(int x, int y)
{

}

void MainScene::onMouseMove(int x, int y)
{
	//if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
#include "MainScene.h"
#include "Application.h"
#include "d3dx12.h"
#include "d3dUtil.h"
#include "UploadBuffer.h"
#include "GameTimer.h"
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <array>
#include "FrameResourceController.h"
#include "PipleneStateData.h"
#include "ObjectsDataProvider.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


MainScene::MainScene(
	std::weak_ptr<Application> appContext,
	std::unique_ptr<PipleneStateData> pipleneStateData,
	std::shared_ptr<FrameResourceController> frameResourceController,
	std::shared_ptr<ObjectsDataProvider> objectsDataProvider) :

	appContext{ appContext },
	pipleneStateData { std::move(pipleneStateData) },
	frameResourceController { frameResourceController },
	objectsDataProvider{ objectsDataProvider },

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

	auto allRitems = objectsDataProvider->renderItems();

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



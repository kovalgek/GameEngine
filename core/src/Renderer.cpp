#include "Renderer.h"
#include "d3dx12.h"
#include "d3dUtil.h"
#include "UploadBuffer.h"
#include "GameTimer.h"
#include "FrameResourceController.h"
#include "Material.h"
#include "FrameResource.h"
#include "SrvHeapProvider.h"
#include "ViewController.h"
#include "PSOProvider.h"

#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <D3Dcompiler.h>
#include <array>

#include "GPUService.h"

#include <dxgi1_4.h>
#include "MeshGeometry.h"

#include <entt.hpp>
#include "Scene.h"


using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

Renderer::Renderer(
	GPUService& gpuService,

	D3D_DRIVER_TYPE d3dDriverType,
	DXGI_FORMAT backBufferFormat,
	DXGI_FORMAT depthStencilFormat,
	bool      msaa4xState,
	UINT      msaa4xQuality,
	UINT rtvDescriptorSize,
	UINT dsvDescriptorSize,
	UINT cbvSrvUavDescriptorSize,

	ComPtr<IDXGISwapChain> swapChain,

	ComPtr<ID3D12DescriptorHeap> rtvHeap,
	ComPtr<ID3D12DescriptorHeap> dsvHeap,

	ComPtr<ID3D12RootSignature> rootSignature,

	std::unique_ptr<PSOProvider> psoProvider,
	FrameResourceController& frameResourceController,
	std::unique_ptr <SrvHeapProvider> srvHeapProvider,
	ViewController& viewController,
	Scene& scene
) :
	gpuService{ gpuService },
	device { gpuService.getDevice() },
	commandQueue{ gpuService.getCommandQueue() },
	commandAllocator{ gpuService.getCommandAllocator() },
	commandList{ gpuService.getCommandList() },
	fence { gpuService.getFence() },

	d3dDriverType{ d3dDriverType },
	backBufferFormat{ backBufferFormat },
	depthStencilFormat{ depthStencilFormat },
	msaa4xState { msaa4xState },
	msaa4xQuality{ msaa4xQuality },
	rtvDescriptorSize { rtvDescriptorSize },
	dsvDescriptorSize { dsvDescriptorSize },
	cbvSrvUavDescriptorSize { cbvSrvUavDescriptorSize },

	swapChain{ swapChain },
	rtvHeap { rtvHeap },
	dsvHeap { dsvHeap },

	rootSignature{ rootSignature },

	psoProvider{ std::move(psoProvider) },
	frameResourceController { frameResourceController },
	srvHeapProvider{ std::move(srvHeapProvider) },
	viewController{ viewController },
	scene { scene }
{

}

Renderer::~Renderer() = default;

void Renderer::onKeyboardInput(const GameTimer& gameTimer)
{
	if (GetAsyncKeyState('1') & 0x8000)
		isWireframe = true;
	else
		isWireframe = false;
}
bool show_demo_window = true;

void Renderer::draw(const GameTimer& gameTimer)
{
	auto cmdListAlloc = frameResourceController.getCurrentFrameResource()->CmdListAlloc;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(commandList->Reset(cmdListAlloc.Get(), psoProvider->getPipelineStateObject("opaque")));

	commandList->RSSetViewports(1, &screenViewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	// Indicate a state transition on the resource usage.
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	commandList->ClearRenderTargetView(currentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	commandList->ClearDepthStencilView(depthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	commandList->OMSetRenderTargets(1, &currentBackBufferView(), true, &depthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeapProvider->getSrvDescriptorHeap()};
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetGraphicsRootSignature(rootSignature.Get());

	auto passCB = frameResourceController.getCurrentFrameResource()->PassCB->Resource();
	commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	auto renderView = scene.renderView();
	drawRenderItems(commandList, renderView);

	//// Mark the visible mirror pixels in the stencil buffer with the value 1
	//commandList->OMSetStencilRef(1);
	//commandList->SetPipelineState(psoProvider->getPipelineStateObject("markStencilMirrors"));
	//auto mirrorsRenderItems = objectsDataProvider.renderItemsForLayer(RenderLayer::Mirrors);
	//drawRenderItems(commandList, mirrorsRenderItems);

	//// Draw the reflection into the mirror only (only for pixels where the stencil buffer is 1).
	//// Note that we must supply a different per-pass constant buffer--one with the lights reflected.
	//UINT passCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(PassConstants));
	//commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress() + 1 * passCBByteSize);
	//commandList->SetPipelineState(psoProvider->getPipelineStateObject("drawStencilReflections"));
	//auto reflectedRenderItems = objectsDataProvider.renderItemsForLayer(RenderLayer::Reflected);
	//drawRenderItems(commandList, reflectedRenderItems);

	//// Restore main pass constants and stencil ref.
	//commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	//commandList->OMSetStencilRef(0);

	//commandList->SetPipelineState(psoProvider->getPipelineStateObject("alphaTested"));
	//auto alphaTestedRenderItems = objectsDataProvider.renderItemsForLayer(RenderLayer::AlphaTested);
	//drawRenderItems(commandList, alphaTestedRenderItems);

	//commandList->SetPipelineState(psoProvider->getPipelineStateObject("transparent"));
	//auto transparentRenderItems = objectsDataProvider.renderItemsForLayer(RenderLayer::Transparent);
	//drawRenderItems(commandList, transparentRenderItems);

	//// Draw shadows
	//commandList->SetPipelineState(psoProvider->getPipelineStateObject("shadow"));
	//auto shadowRenderItems = objectsDataProvider.renderItemsForLayer(RenderLayer::Shadow);
	//drawRenderItems(commandList, shadowRenderItems);

	viewController.present();
	viewController.update();

	// Indicate a state transition on the resource usage.
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(currentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(commandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	swapChainMethod();

	//// Advance the fence value to mark commands up to this fence point.
	frameResourceController.getCurrentFrameResource()->Fence = gpuService.setNewFenceOnGPUTimeline();
}

void Renderer::drawRenderItems(ID3D12GraphicsCommandList* cmdList, RenderView renderView)
{
	UINT objCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = frameResourceController.getCurrentFrameResource()->ObjectCB->Resource();
	auto matCB = frameResourceController.getCurrentFrameResource()->MaterialCB->Resource();

	for (auto entity : renderView) {

		auto& renderComponent = renderView.get<RenderComponent>(entity);
		
		cmdList->IASetVertexBuffers(0, 1, &renderComponent.geometry->VertexBufferView());
		cmdList->IASetIndexBuffer(&renderComponent.geometry->IndexBufferView());
		cmdList->IASetPrimitiveTopology(renderComponent.primitiveType);

		auto tex = srvHeapProvider->getHandleForIndex(renderComponent.material->DiffuseSrvHeapIndex);
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + renderComponent.objectConstantBufferIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + renderComponent.material->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootDescriptorTable(0, tex);
		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		cmdList->DrawIndexedInstanced(renderComponent.indexCount, 1, renderComponent.startIndexLocation, renderComponent.baseVertexLocation, 0);
	}
}

ID3D12Resource* Renderer::currentBackBuffer()const
{
	return swapChainBuffer[currBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::currentBackBufferView()const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		currBackBuffer,
		rtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::depthStencilView()const
{
	return dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void Renderer::onResize(int clientWidth, int clientHeight)
{
	this->clientWidth = clientWidth;
	this->clientHeight = clientHeight;

	assert(device);
	assert(swapChain);
	assert(commandAllocator);

	// Flush before changing any resources.
	gpuService.flushCommandQueue();

	ThrowIfFailed(commandList->Reset(commandAllocator, nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < swapChainBufferCount; ++i)
	{
		swapChainBuffer[i].Reset();
	}
	depthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(swapChain->ResizeBuffers(
		swapChainBufferCount,
		clientWidth, clientHeight,
		backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	currBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < swapChainBufferCount; i++)
	{
		ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainBuffer[i])));
		device->CreateRenderTargetView(swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, rtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = clientWidth;
	depthStencilDesc.Height = clientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = msaa4xState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = msaa4xState ? (msaa4xQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(depthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = depthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, depthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Execute the resize commands.
	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	gpuService.flushCommandQueue();

	// Update the viewport transform to cover the client area.
	screenViewport.TopLeftX = 0;
	screenViewport.TopLeftY = 0;
	screenViewport.Width = static_cast<float>(clientWidth);
	screenViewport.Height = static_cast<float>(clientHeight);
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;

	scissorRect = { 0, 0, clientWidth, clientHeight };
}

void Renderer::swapChainMethod()
{
	// swap the back and front buffers
	ThrowIfFailed(swapChain->Present(0, 0));
	currBackBuffer = (currBackBuffer + 1) % Renderer::swapChainBufferCount;
}
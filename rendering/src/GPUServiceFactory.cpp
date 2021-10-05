#include "GPUServiceFactory.h"
#include <dxgi1_4.h>
#include "GPUService.h"
#include "d3dUtil.h"

using Microsoft::WRL::ComPtr;

std::unique_ptr<GPUService> GPUServiceFactory::getGPUService(IDXGIFactory4* dxgiFactory)
{
	ComPtr<ID3D12Device> d3dDevice;
	GPUServiceFactory::createDevice(dxgiFactory, d3dDevice.GetAddressOf());

	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	GPUServiceFactory::createCommandObjects(
		d3dDevice.Get(),
		commandQueue.GetAddressOf(),
		commandAllocator.GetAddressOf(),
		commandList.GetAddressOf()
	);

	ComPtr<ID3D12Fence> fence;
	GPUServiceFactory::createFence(d3dDevice.Get(), fence.GetAddressOf());

	return std::make_unique<GPUService>(
		d3dDevice,
		commandQueue,
		commandAllocator,
		commandList,
		fence);
}

void GPUServiceFactory::createDevice(IDXGIFactory4* dxgiFactory, ID3D12Device** d3dDevice)
{
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(d3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(d3dDevice)));
	}
}

void GPUServiceFactory::createCommandObjects(
	ID3D12Device* d3dDevice,
	ID3D12CommandQueue** commandQueue,
	ID3D12CommandAllocator** commandAllocator,
	ID3D12GraphicsCommandList** commandList
)
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue)));

	ThrowIfFailed(d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(commandAllocator)));

	ThrowIfFailed(d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		*commandAllocator, // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(commandList)));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	(*commandList)->Close();
}

void GPUServiceFactory::createFence(ID3D12Device* d3dDevice, ID3D12Fence** fence)
{
	ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence)));
}

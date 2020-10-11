#include "RendererFactory.h"
#include "Renderer.h"
#include <dxgi1_4.h>
#include <d3d12.h>
#include <wrl.h>
#include "SrvHeapProvider.h"
#include "PipleneStateData.h"
#include "d3dUtil.h"
#include "GPUService.h"

using Microsoft::WRL::ComPtr;

std::unique_ptr<Renderer> RendererFactory::getRenderer(
	HWND mainWindowHandle,
	GPUService* gpuService,
	FrameResourceController* frameResourceController,
	ObjectsDataProvider* objectsDataProvider,
	std::unique_ptr <SrvHeapProvider> srvHeapProvider,
	ViewController* viewController
)
{
	ComPtr<IDXGIFactory4> dxgiFactory;
	RendererFactory::createDXGIFactory(dxgiFactory.GetAddressOf());

	ComPtr<ID3D12Fence> fence;
	RendererFactory::createFence(gpuService->getDevice(), fence.GetAddressOf());

	D3D_DRIVER_TYPE d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	bool      msaa4xState = false;
	UINT      msaa4xQuality = 0;


	RendererFactory::setUp4xMSAAQuality(
		gpuService->getDevice(),
		backBufferFormat,
		msaa4xQuality
	);
	
	int swapChainBufferCount = 2;

	ComPtr<IDXGISwapChain> swapChain;
	RendererFactory::createSwapChain(
		dxgiFactory.Get(),
		gpuService->getCommandQueue(),
		backBufferFormat,
		msaa4xState,
		msaa4xQuality,
		swapChainBufferCount,
		mainWindowHandle,
		swapChain.GetAddressOf()
	);

	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	RendererFactory::createRtvAndDsvDescriptorHeaps(
		gpuService->getDevice(),
		swapChainBufferCount,
		rtvHeap.GetAddressOf(),
		dsvHeap.GetAddressOf()
	);

	UINT rtvDescriptorSize = 0;
	UINT dsvDescriptorSize = 0;
	UINT cbvSrvUavDescriptorSize = 0;
	RendererFactory::setUpDescriptorSizes(
		gpuService->getDevice(),
		rtvDescriptorSize,
		dsvDescriptorSize,
		cbvSrvUavDescriptorSize
	);

	auto pipleneStateData = std::make_unique<PipleneStateData>(
		gpuService->getDevice(),
		backBufferFormat,
		depthStencilFormat,
		msaa4xState,
		msaa4xQuality
	);

	return std::make_unique<Renderer>(
		gpuService,
		d3dDriverType,
		backBufferFormat,
		depthStencilFormat,
		msaa4xState,
		msaa4xQuality,
		rtvDescriptorSize,
		dsvDescriptorSize,
		cbvSrvUavDescriptorSize,
		swapChain,
		rtvHeap,
		dsvHeap,
		std::move(pipleneStateData),
		frameResourceController,
		objectsDataProvider,
		std::move(srvHeapProvider),
		viewController);
}

void RendererFactory::createDXGIFactory(IDXGIFactory4 **dxgiFactory)
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory)));
}

void RendererFactory::createDevice(IDXGIFactory4 *dxgiFactory, ID3D12Device **d3dDevice)
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

void RendererFactory::createCommandObjects(
	ID3D12Device* d3dDevice,
	ID3D12CommandQueue **commandQueue,
	ID3D12CommandAllocator **commandAllocator,
	ID3D12GraphicsCommandList **commandList
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

void RendererFactory::createFence(ID3D12Device* d3dDevice, ID3D12Fence **fence)
{
	ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence)));
}

void RendererFactory::setUpDescriptorSizes(
	ID3D12Device* d3dDevice,
	UINT &rtvDescriptorSize,
	UINT &dsvDescriptorSize,
	UINT &cbvSrvUavDescriptorSize
)
{
	rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	cbvSrvUavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RendererFactory::setUp4xMSAAQuality(ID3D12Device* d3dDevice, DXGI_FORMAT backBufferFormat, UINT &msaa4xQuality)
{
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = backBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	msaa4xQuality = msQualityLevels.NumQualityLevels;
	assert(msaa4xQuality > 0 && "Unexpected MSAA quality level.");
}

void RendererFactory::createSwapChain(
	IDXGIFactory4* dxgiFactory,
	ID3D12CommandQueue *commandQueue,
	DXGI_FORMAT backBufferFormat,
	bool msaa4xState,
	UINT msaa4xQuality,
	int swapChainBufferCount,
	HWND mainWindowHandle,
	IDXGISwapChain** swapChain
)
{
	// Release the previous swapchain we will be recreating.
	//(*swapChain)->Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = 800;
	sd.BufferDesc.Height = 600;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = backBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = msaa4xState ? 4 : 1;
	sd.SampleDesc.Quality = msaa4xState ? (msaa4xQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = swapChainBufferCount;
	sd.OutputWindow = mainWindowHandle;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(dxgiFactory->CreateSwapChain(
		commandQueue,
		&sd,
		swapChain));
}

void RendererFactory::createRtvAndDsvDescriptorHeaps(
	ID3D12Device* d3dDevice,
	int swapChainBufferCount,
	ID3D12DescriptorHeap **rtvHeap,
	ID3D12DescriptorHeap **dsvHeap
)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = swapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(rtvHeap)));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap)));
}
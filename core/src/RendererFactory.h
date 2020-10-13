#include <memory>
#include <wrl.h>
#include <windows.h>

class Renderer;

class GPUService;
class FrameResourceController;
class ObjectsDataProvider;
class SrvHeapProvider;
class ViewController;
struct IDXGIFactory4;
struct IDXGISwapChain;
struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;
enum DXGI_FORMAT;
struct ID3D12DescriptorHeap;

#pragma once
class RendererFactory
{
public:
	static std::unique_ptr<Renderer> getRenderer(
		HWND mainWindowHandle,
		GPUService &gpuService,
		FrameResourceController& frameResourceController,
		ObjectsDataProvider& objectsDataProvider,
		std::unique_ptr <SrvHeapProvider> srvHeapProvider,
		ViewController& viewController
	);
private:
	static void RendererFactory::createDXGIFactory(IDXGIFactory4** dxgiFactory);
	static void RendererFactory::createDevice(IDXGIFactory4* dxgiFactory, ID3D12Device** d3dDevice);
	static void RendererFactory::createCommandObjects(
		ID3D12Device* d3dDevice,
		ID3D12CommandQueue** commandQueue,
		ID3D12CommandAllocator** commandAllocator,
		ID3D12GraphicsCommandList** commandList
	);
	static void createFence(ID3D12Device* d3dDevice, ID3D12Fence** fence);
	static void setUp4xMSAAQuality(ID3D12Device* d3dDevice, DXGI_FORMAT backBufferFormat, UINT& msaa4xQuality);

	static void createSwapChain(
		IDXGIFactory4* dxgiFactory,
		ID3D12CommandQueue* commandQueue,
		DXGI_FORMAT backBufferFormat,
		bool msaa4xState,
		UINT msaa4xQuality,
		int swapChainBufferCount,
		HWND mainWindowHandle,
		IDXGISwapChain** swapChain
	);

	static void RendererFactory::createRtvAndDsvDescriptorHeaps(
		ID3D12Device* d3dDevice,
		int swapChainBufferCount,
		ID3D12DescriptorHeap** rtvHeap,
		ID3D12DescriptorHeap** dsvHeap
	);

	static void setUpDescriptorSizes(
		ID3D12Device* d3dDevice,
		UINT& rtvDescriptorSize,
		UINT& dsvDescriptorSize,
		UINT& cbvSrvUavDescriptorSize
	);
};


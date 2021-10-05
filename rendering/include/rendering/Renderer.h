#include <wrl/client.h>
#include <vector>
#include <memory>
#include <string>
#include <d3d12.h>

class GPUService;
class GameTimer;

#include "Scene.h"

class SrvHeapProvider;
class ViewController;
class PSOProvider;
struct FrameResource;
struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;
struct IDXGISwapChain;

const int gNumFrameResources = 3;

#pragma once
class Renderer
{
public:
	Renderer(
		GPUService& gpuService,

		D3D_DRIVER_TYPE d3dDriverType,
		DXGI_FORMAT backBufferFormat,
		DXGI_FORMAT depthStencilFormat,
		bool      msaa4xState,
		UINT      msaa4xQuality,

		UINT rtvDescriptorSize,
		UINT dsvDescriptorSize,
		UINT cbvSrvUavDescriptorSize,

		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain,

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap,

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature,

		std::unique_ptr<PSOProvider> psoProvider,

		std::unique_ptr<SrvHeapProvider> srvHeapProvider,
		ViewController& viewController,
		Scene &scene
	);

	~Renderer();
	void draw(FrameResource& frameResource, const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);



	// Properties for PSO
	DXGI_FORMAT getBackBufferFormat() const { return backBufferFormat; }
	DXGI_FORMAT getDepthStencilFormat() const { return depthStencilFormat; }
	bool getMsaa4xState() const { return msaa4xState; }
	UINT getMsaa4xQuality() const { return msaa4xQuality; }
	int getClientWidth() const { return clientWidth; }
	int getClientHeight() const { return clientHeight; }

private:

	GPUService& gpuService;


	std::unique_ptr<PSOProvider> psoProvider;
	std::unique_ptr <SrvHeapProvider> srvHeapProvider;
	ViewController& viewController;
	Scene& scene;

	void onKeyboardInput(const GameTimer& gameTimer);
	void drawRenderItems(ID3D12GraphicsCommandList* cmdList, RenderView renderView, FrameResource& frameResource);

	bool isWireframe = false;


	ID3D12Device *device;
	ID3D12CommandQueue *commandQueue;
	ID3D12CommandAllocator *commandAllocator;
	ID3D12GraphicsCommandList *commandList;
	ID3D12Fence *fence;

	D3D_DRIVER_TYPE d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bool      msaa4xState = false;
	UINT      msaa4xQuality = 0;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	UINT rtvDescriptorSize = 0;
	UINT dsvDescriptorSize = 0;
	UINT cbvSrvUavDescriptorSize = 0;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	ID3D12Resource* currentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView()const;


	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	int currBackBuffer = 0;
	static const int swapChainBufferCount = 2;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer[swapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
	void swapChainMethod();


	int clientWidth = 800;
	int clientHeight = 600;
	D3D12_VIEWPORT screenViewport;
	D3D12_RECT scissorRect;
};


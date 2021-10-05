#include <memory>

class GPUService;
struct IDXGIFactory4;
struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

#pragma once
class GPUServiceFactory
{
public:
	static std::unique_ptr<GPUService> getGPUService(IDXGIFactory4* dxgiFactory);
private:
	static void createDevice(IDXGIFactory4* dxgiFactory, ID3D12Device** d3dDevice);
	static void createCommandObjects(
		ID3D12Device* d3dDevice,
		ID3D12CommandQueue** commandQueue,
		ID3D12CommandAllocator** commandAllocator,
		ID3D12GraphicsCommandList** commandList
	);
	static void createFence(ID3D12Device* d3dDevice, ID3D12Fence** fence);
};


#include <wrl/client.h>

struct ID3D12Device;
struct ID3D12CommandQueue;
struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

#pragma once
class GPUService
{
public:
	GPUService(
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList,
		Microsoft::WRL::ComPtr<ID3D12Fence> fence);

	~GPUService();

	// Factory for directx objects
	ID3D12Device* getDevice() const { return device.Get(); }

	// Execute commands
	ID3D12GraphicsCommandList* getCommandList() const { return commandList.Get(); }
	ID3D12CommandAllocator* getCommandAllocator() const { return commandAllocator.Get(); }
	ID3D12CommandQueue* getCommandQueue() const { return commandQueue.Get(); }
	ID3D12Fence* getFence() const { return fence.Get(); }

	void flushCommandQueue();
	UINT64 setNewFenceOnGPUTimeline();
	void waitForGPUFence(UINT64 currentFence);
	UINT64 currentFence = 0;

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
};


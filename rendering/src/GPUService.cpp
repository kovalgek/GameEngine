#include "GPUService.h"
#include "d3dUtil.h"

using Microsoft::WRL::ComPtr;

GPUService::GPUService(
	ComPtr<ID3D12Device> device,
	ComPtr<ID3D12CommandQueue> commandQueue,
	ComPtr<ID3D12CommandAllocator> commandAllocator,
	ComPtr<ID3D12GraphicsCommandList> commandList,
	ComPtr<ID3D12Fence> fence
) : 
	device{ device },
	commandQueue{ commandQueue },
	commandAllocator{ commandAllocator },
	commandList{ commandList },
	fence{ fence }
{

}

GPUService::~GPUService() = default;

void GPUService::flushCommandQueue()
{
	setNewFenceOnGPUTimeline();
	waitForGPUFence(currentFence);
}

UINT64 GPUService::setNewFenceOnGPUTimeline()
{
	// Advance the fence value to mark commands up to this fence point.
	currentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(commandQueue->Signal(fence.Get(), currentFence));

	return currentFence;
}

void GPUService::waitForGPUFence(UINT64 currentFence)
{
	// Wait until the GPU has completed commands up to this fence point.
	if (currentFence != 0 && fence->GetCompletedValue() < currentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(fence->SetEventOnCompletion(currentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
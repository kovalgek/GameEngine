#include "FrameResourceController.h"
#include "FrameResource.h"
#include "UploadBuffer.h"
#include <d3d12.h>

FrameResourceController::FrameResourceController(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount) :
	device { device },
	passCount { passCount },
	objectCount { objectCount }
{
	for (int i = 0; i < kFrameResourcesCount; ++i)
	{
		frameResources.push_back(std::make_unique<FrameResource>(device, passCount, objectCount, materialCount, waveVertCount));
	}

	//cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//createDescriptorHeaps();
	//createConstantBufferViews();
}

FrameResourceController::~FrameResourceController()
{

}

void FrameResourceController::changeFrameResource()
{
	index = (index + 1) % kFrameResourcesCount;
}

FrameResource* FrameResourceController::getCurrentFrameResource()
{
	return frameResources[index].get();
}

void FrameResourceController::createDescriptorHeaps()
{
	// Need a CBV descriptor for each object for each frame resource,
	// +1 for the perPass CBV for each frame resource.
	UINT numDescriptors = (objectCount + 1) * kFrameResourcesCount;

	// Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
	passCbvOffset = objectCount * kFrameResourcesCount;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvHeap)));
}

void FrameResourceController::createConstantBufferViews()
{
	UINT objCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(ObjectConstants));

	// Need a CBV descriptor for each object for each frame resource.
	for (int frameIndex = 0; frameIndex < kFrameResourcesCount; ++frameIndex)
	{
		auto objectCB = frameResources[frameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < objectCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

			// Offset to the ith object constant buffer in the buffer.
			cbAddress += i * objCBByteSize;

			// Offset to the object cbv in the descriptor heap.
			int heapIndex = frameIndex * objectCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, cbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;

			device->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT passCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(PassConstants));

	// Last three descriptors are the pass CBVs for each frame resource.
	for (int frameIndex = 0; frameIndex < kFrameResourcesCount; ++frameIndex)
	{
		auto passCB = frameResources[frameIndex]->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		// Offset to the pass cbv in the descriptor heap.
		int heapIndex = passCbvOffset + frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, cbvSrvUavDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		device->CreateConstantBufferView(&cbvDesc, handle);
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE FrameResourceController::passCbvHandle()
{
	int passCbvIndex = passCbvOffset + index;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, cbvSrvUavDescriptorSize);
	return passCbvHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE FrameResourceController::cbvHandle(UINT objCBIndex)
{
	UINT cbvIndex = index * objectCount + objCBIndex;
	auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(cbvHeap->GetGPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(cbvIndex, cbvSrvUavDescriptorSize);
	return cbvHandle;
}
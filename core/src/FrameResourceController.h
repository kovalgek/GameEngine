#include <memory>
#include <vector>
#include <wrl/client.h>

struct ID3D12Device;
struct ID3D12DescriptorHeap;
struct FrameResource;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

const int kFrameResourcesCount = 3;

#pragma once
class FrameResourceController
{
public:
	FrameResourceController(
		ID3D12Device* device,
		UINT passCount,
		UINT objectCount,
		UINT materialCount
	);
	~FrameResourceController();
	void changeFrameResource();

	FrameResource* getCurrentFrameResource();

	ID3D12DescriptorHeap* getCbvHeap() const { return  cbvHeap.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE passCbvHandle();
	D3D12_GPU_DESCRIPTOR_HANDLE cbvHandle(UINT objCBIndex);

private:
	void createDescriptorHeaps();
	void createConstantBufferViews();

	ID3D12Device* device;
	UINT passCount;
	UINT objectCount;

	std::vector<std::unique_ptr<FrameResource>> frameResources;
	int index = 0;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap = nullptr;
	UINT passCbvOffset = 0;
	UINT cbvSrvUavDescriptorSize = 0;
};


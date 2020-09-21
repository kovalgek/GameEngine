#include <string>
#include <memory>
#include <wrl/client.h>

struct D3D12_GPU_DESCRIPTOR_HANDLE;
struct ID3D12Device;
struct ID3D12DescriptorHeap;
class TexturesProvider;




#pragma once
class SrvHeapProvider
{
public:
	SrvHeapProvider(ID3D12Device* device, std::unique_ptr<TexturesProvider> texturesProvider);
	~SrvHeapProvider();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE getHandleForIndex(UINT index);
	ID3D12DescriptorHeap* getSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }

private:
	// Create the SRV heap.
	void createHeap();
	// Fill out the heap with actual descriptors.
	void createViews();

	ID3D12Device* device;
	std::unique_ptr<TexturesProvider> texturesProvider;

	// Uses for descriptors
	UINT cbvSrvDescriptorSize = 0;
};


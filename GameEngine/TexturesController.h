#include <unordered_map>
#include <string>
#include <memory>
#include <wrl/client.h>

struct D3D12_GPU_DESCRIPTOR_HANDLE;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct Texture;
struct ID3D12DescriptorHeap;

#pragma once
class TexturesController
{
	void loadTextures();

	// Create the SRV heap.
	void createHeap();
	// Fill out the heap with actual descriptors.
	void createViews();


	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;
	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;


public:
	UINT cbvSrvDescriptorSize = 0;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	TexturesController(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList);
	~TexturesController();

	D3D12_GPU_DESCRIPTOR_HANDLE getHandleForIndex(UINT index);
	ID3D12DescriptorHeap* getSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }
};


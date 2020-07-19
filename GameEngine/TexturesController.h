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
	// Load all used textures to memory
	void loadTextures();
	// Create the SRV heap.
	void createHeap();
	// Fill out the heap with actual descriptors.
	void createViews();

	std::unique_ptr<Texture> grassTexture();
	std::unique_ptr<Texture> waterTexture();
	std::unique_ptr<Texture> fenceTexture();

	// Help loading textures
	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;

	// Uses for descriptors
	UINT cbvSrvDescriptorSize = 0;

	std::unordered_map<std::string, std::unique_ptr<Texture>> textures;

public:
	TexturesController(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList);
	~TexturesController();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE getHandleForIndex(UINT index);
	ID3D12DescriptorHeap* getSrvDescriptorHeap() { return srvDescriptorHeap.Get(); }
};


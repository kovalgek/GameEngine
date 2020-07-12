#include "TexturesController.h"
#include "d3dUtil.h"
#include "DDSTextureLoader.h"
#include "Texture.h"

TexturesController::TexturesController(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) : device { device }, commandList { commandList }
{
	// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
	// so we have to query this information.
	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	loadTextures();
	createHeap();
	createViews();
}

TexturesController::~TexturesController() = default;

void TexturesController::loadTextures()
{
	textures["grassTex"] = grassTexture();
	textures["waterTex"] = waterTexture();
	textures["fenceTex"] = fenceTexture();
}

std::unique_ptr<Texture> TexturesController::grassTexture()
{
	auto grassTex = std::make_unique<Texture>();
	grassTex->Name = "grassTex";
	grassTex->Filename = L"Textures/grass.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		device,
		commandList,
		grassTex->Filename.c_str(),
		grassTex->Resource,
		grassTex->UploadHeap)
	);
	return grassTex;
}

std::unique_ptr<Texture> TexturesController::waterTexture()
{
	auto waterTex = std::make_unique<Texture>();
	waterTex->Name = "waterTex";
	waterTex->Filename = L"Textures/water1.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		device,
		commandList,
		waterTex->Filename.c_str(),
		waterTex->Resource,
		waterTex->UploadHeap)
	);
	return waterTex;
}

std::unique_ptr<Texture> TexturesController::fenceTexture()
{
	auto fenceTex = std::make_unique<Texture>();
	fenceTex->Name = "fenceTex";
	fenceTex->Filename = L"Textures/WoodCrate01.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
		device,
		commandList,
		fenceTex->Filename.c_str(),
		fenceTex->Resource,
		fenceTex->UploadHeap)
	);
	return fenceTex;
}

void TexturesController::createHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap)));
}

void TexturesController::createViews()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	auto grassTex = textures["grassTex"]->Resource;
	auto waterTex = textures["waterTex"]->Resource;
	auto fenceTex = textures["fenceTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = grassTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	device->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, cbvSrvDescriptorSize);

	srvDesc.Format = waterTex->GetDesc().Format;
	device->CreateShaderResourceView(waterTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, cbvSrvDescriptorSize);

	srvDesc.Format = fenceTex->GetDesc().Format;
	device->CreateShaderResourceView(fenceTex.Get(), &srvDesc, hDescriptor);
}

D3D12_GPU_DESCRIPTOR_HANDLE TexturesController::getHandleForIndex(UINT index)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(index, cbvSrvDescriptorSize);
	return tex;
}
#include "SrvHeapProvider.h"
#include "d3dUtil.h"
#include "DDSTextureLoader.h"
#include "Texture.h"
#include "TexturesProvider.h"

SrvHeapProvider::SrvHeapProvider(ID3D12Device* device, std::unique_ptr<TexturesProvider> texturesProvider) : device { device }, texturesProvider { std::move(texturesProvider) }
{
	createHeap();
	createViews();
}

SrvHeapProvider::~SrvHeapProvider() = default;

void SrvHeapProvider::createHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	auto textures = texturesProvider->getTextureResources();
	srvHeapDesc.NumDescriptors = textures.size() + 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap)));
}

void SrvHeapProvider::createViews()
{
	// Get the increment size of a descriptor in this heap type.  This is hardware specific, 
	// so we have to query this information.
	cbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hDescriptor.Offset(1, cbvSrvDescriptorSize);

	for (auto texture : texturesProvider->getTextureResources())
	{
		srvDesc.Format = texture->GetDesc().Format;
		device->CreateShaderResourceView(texture, &srvDesc, hDescriptor);
		hDescriptor.Offset(1, cbvSrvDescriptorSize);
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvHeapProvider::getHandleForIndex(UINT index)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(index + 1, cbvSrvDescriptorSize);
	return tex;
}
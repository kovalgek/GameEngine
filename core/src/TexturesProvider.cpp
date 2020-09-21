#include "TexturesProvider.h"
#include "DDSTextureLoader.h"
#include "Texture.h"
#include "d3dUtil.h"

using namespace DirectX;

TexturesProvider::TexturesProvider(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) : device{ device }, commandList{ commandList }
{
	loadTextures();
}

TexturesProvider::~TexturesProvider() = default;

void TexturesProvider::loadTextures()
{
	textures = std::vector<std::unique_ptr<Texture>>();

	textures.push_back(grassTexture());
	textures.push_back(waterTexture());
	textures.push_back(fenceTexture());
	textures.push_back(white1x1Texture());
}

std::vector<ID3D12Resource*> TexturesProvider::getTextureResources()
{
	std::vector<ID3D12Resource*> resources;

	for (auto &element : textures)
	{
		resources.push_back(element->Resource.Get());
	}

	return resources;
}

std::unique_ptr<Texture> TexturesProvider::grassTexture()
{
	auto grassTex = std::make_unique<Texture>();
	grassTex->Name = "grassTex";
	grassTex->Filename = L"../core/resources/textures/grass.dds";
	ThrowIfFailed(CreateDDSTextureFromFile12(
		device,
		commandList,
		grassTex->Filename.c_str(),
		grassTex->Resource,
		grassTex->UploadHeap)
	);
	return grassTex;
}

std::unique_ptr<Texture> TexturesProvider::waterTexture()
{
	auto waterTex = std::make_unique<Texture>();
	waterTex->Name = "waterTex";
	waterTex->Filename = L"../core/resources/textures/water1.dds";
	ThrowIfFailed(CreateDDSTextureFromFile12(
		device,
		commandList,
		waterTex->Filename.c_str(),
		waterTex->Resource,
		waterTex->UploadHeap)
	);
	return waterTex;
}

std::unique_ptr<Texture> TexturesProvider::fenceTexture()
{
	auto fenceTex = std::make_unique<Texture>();
	fenceTex->Name = "fenceTex";
	fenceTex->Filename = L"../core/resources/textures/WoodCrate01.dds";
	ThrowIfFailed(CreateDDSTextureFromFile12(
		device,
		commandList,
		fenceTex->Filename.c_str(),
		fenceTex->Resource,
		fenceTex->UploadHeap)
	);
	return fenceTex;
}

std::unique_ptr<Texture> TexturesProvider::white1x1Texture()
{
	auto white1x1Tex = std::make_unique<Texture>();
	white1x1Tex->Name = "white1x1Tex";
	white1x1Tex->Filename = L"../core/resources/textures/white1x1.dds";
	ThrowIfFailed(CreateDDSTextureFromFile12(
		device,
		commandList,
		white1x1Tex->Filename.c_str(),
		white1x1Tex->Resource,
		white1x1Tex->UploadHeap)
	);
	return white1x1Tex;
}
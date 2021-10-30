#include <unordered_map>
#include <memory>

struct Texture;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12Resource;

#pragma once
class TexturesProvider
{
public:
	TexturesProvider(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~TexturesProvider();

	std::vector<ID3D12Resource *> getTextureResources();
private:
	// Help loading textures
	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;

	// Load all used textures to memory
	void loadTextures();

	std::unique_ptr<Texture> grassTexture();
	std::unique_ptr<Texture> waterTexture();
	std::unique_ptr<Texture> fenceTexture();
	std::unique_ptr<Texture> white1x1Texture();
	std::unique_ptr<Texture> iceTexture();
	std::unique_ptr<Texture> checkboardTexture();
	std::unique_ptr<Texture> bricksTexture();

	std::vector<std::unique_ptr<Texture>> textures;
};


#include "MaterialsDataProvider.h"
#include "d3dUtil.h"
#include <DirectXMath.h>
#include "Material.h"

using namespace DirectX;

MaterialsDataProvider::MaterialsDataProvider()
{
	buildMaterials();
}

MaterialsDataProvider::~MaterialsDataProvider() = default;

void MaterialsDataProvider::buildMaterials()
{
	materials["grass"] = grassMaterial();
	materials["water"] = waterMaterial();
	materials["wirefence"] = wirefenceMaterial();
	materials["skullMat"] = skullMaterial();
	materials["bricks"] = bricksMaterial();
	materials["checkertile"] = checkertileMaterial();
	materials["icemirror"] = icemirrorMaterial();
	materials["shadowMat"] = shadowMatMaterial();
}

std::unique_ptr<Material> MaterialsDataProvider::grassMaterial()
{
	auto grass = std::make_unique<Material>();
	grass->Name = "grass";
	grass->MatCBIndex = 0;
	grass->DiffuseSrvHeapIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	grass->Roughness = 0.125f;
	return grass;
}

std::unique_ptr<Material> MaterialsDataProvider::waterMaterial()
{
	auto water = std::make_unique<Material>();
	water->Name = "water";
	water->MatCBIndex = 1;
	water->DiffuseSrvHeapIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;
	return water;
}

std::unique_ptr<Material> MaterialsDataProvider::wirefenceMaterial()
{
	auto wirefence = std::make_unique<Material>();
	wirefence->Name = "wirefence";
	wirefence->MatCBIndex = 2;
	wirefence->DiffuseSrvHeapIndex = 2;
	wirefence->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	wirefence->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	wirefence->Roughness = 0.25f;
	return wirefence;
}

std::unique_ptr<Material> MaterialsDataProvider::skullMaterial()
{
	auto skullMat = std::make_unique<Material>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = 3;
	skullMat->DiffuseSrvHeapIndex = 3;
	skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMat->Roughness = 0.3f;
	return skullMat;
}

std::unique_ptr<Material> MaterialsDataProvider::bricksMaterial()
{
	auto bricks = std::make_unique<Material>();
	bricks->Name = "bricks";
	bricks->MatCBIndex = 4;
	bricks->DiffuseSrvHeapIndex = 0;
	bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	bricks->Roughness = 0.25f;
	return bricks;
}

std::unique_ptr<Material> MaterialsDataProvider::checkertileMaterial()
{
	auto checkertile = std::make_unique<Material>();
	checkertile->Name = "checkertile";
	checkertile->MatCBIndex = 5;
	checkertile->DiffuseSrvHeapIndex = 1;
	checkertile->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	checkertile->FresnelR0 = XMFLOAT3(0.07f, 0.07f, 0.07f);
	checkertile->Roughness = 0.3f;
	return checkertile;
}

std::unique_ptr<Material> MaterialsDataProvider::icemirrorMaterial()
{
	auto icemirror = std::make_unique<Material>();
	icemirror->Name = "icemirror";
	icemirror->MatCBIndex = 6;
	icemirror->DiffuseSrvHeapIndex = 2;
	icemirror->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	icemirror->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	icemirror->Roughness = 0.5f;
	return icemirror;
}

std::unique_ptr<Material> MaterialsDataProvider::shadowMatMaterial()
{
	auto shadowMat = std::make_unique<Material>();
	shadowMat->Name = "shadowMat";
	shadowMat->MatCBIndex = 7;
	shadowMat->DiffuseSrvHeapIndex = 3;
	shadowMat->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	shadowMat->FresnelR0 = XMFLOAT3(0.001f, 0.001f, 0.001f);
	shadowMat->Roughness = 0.0f;
	return shadowMat;
}

Material* MaterialsDataProvider::getMaterialForName(std::string name)
{
	return materials[name].get();
}

std::vector<Material*> MaterialsDataProvider::getMaterials()
{
	std::vector<Material*> result;
	for (auto& material : materials)
	{
		result.push_back(material.second.get());
	}
	return result;
}

std::vector <std::string> MaterialsDataProvider::getMaterialNames()
{
	std::vector<std::string> result;
	for (auto& material : materials)
	{
		result.push_back(material.first);
	}
	return result;
}
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
	water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
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
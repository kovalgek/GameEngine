#include "MaterialsDataProvider.h"
#include "d3dUtil.h"
#include <DirectXMath.h>
#include "Material.h"

using namespace DirectX;

MaterialsDataProvider::MaterialsDataProvider() = default;
MaterialsDataProvider::~MaterialsDataProvider() = default;

void MaterialsDataProvider::addMaterial(std::string name, std::unique_ptr<Material> material)
{
	materials[name] = std::move(material);
}

Material* MaterialsDataProvider::getMaterialForName(std::string name) const
{
	return  materials.find(name)->second.get();
}

std::vector<Material*> MaterialsDataProvider::getMaterials() const
{
	std::vector<Material*> result;
	for (auto& material : materials)
	{
		result.push_back(material.second.get());
	}
	return result;
}

std::vector <std::string> MaterialsDataProvider::getMaterialNames() const
{
	std::vector<std::string> result;
	for (auto& material : materials)
	{
		result.push_back(material.first);
	}
	return result;
}
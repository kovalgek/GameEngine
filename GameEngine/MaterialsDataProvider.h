#include <unordered_map>
#include <memory>
#include <vector>

struct Material;

#pragma once
class MaterialsDataProvider
{
public:
	MaterialsDataProvider();
	~MaterialsDataProvider();
	std::vector <Material*> getMaterials();
	Material *getMaterialForName(std::string name);
private:
	void buildMaterials();
	std::unique_ptr<Material> grassMaterial();
	std::unique_ptr<Material> waterMaterial();
	std::unique_ptr<Material> wirefenceMaterial();
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};


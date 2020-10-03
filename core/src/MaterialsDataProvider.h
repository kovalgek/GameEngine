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
	std::vector <std::string> getMaterialNames();

	Material *getMaterialForName(std::string name);
private:
	void buildMaterials();

	std::unique_ptr<Material> grassMaterial();
	std::unique_ptr<Material> waterMaterial();
	std::unique_ptr<Material> wirefenceMaterial();
	std::unique_ptr<Material> skullMaterial();
	std::unique_ptr<Material> bricksMaterial();
	std::unique_ptr<Material> checkertileMaterial();
	std::unique_ptr<Material> icemirrorMaterial();
	std::unique_ptr<Material> shadowMatMaterial();

	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};


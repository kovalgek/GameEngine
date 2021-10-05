#include <memory>

class MaterialsDataProvider;
struct Material;

#pragma once
class MaterialsDataProviderConfigurator
{
public:
	void configure(MaterialsDataProvider& materialsDataProvider);
private:
	std::unique_ptr<Material> grassMaterial();
	std::unique_ptr<Material> waterMaterial();
	std::unique_ptr<Material> wirefenceMaterial();
	std::unique_ptr<Material> skullMaterial();
	std::unique_ptr<Material> bricksMaterial();
	std::unique_ptr<Material> checkertileMaterial();
	std::unique_ptr<Material> icemirrorMaterial();
	std::unique_ptr<Material> shadowMatMaterial();
};


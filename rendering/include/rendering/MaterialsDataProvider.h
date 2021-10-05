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

	void addMaterial(std::string name, std::unique_ptr<Material> material);

	std::vector<Material*> getMaterials() const;
	std::vector<std::string> getMaterialNames() const;
	Material *getMaterialForName(std::string name) const;

private:
	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};


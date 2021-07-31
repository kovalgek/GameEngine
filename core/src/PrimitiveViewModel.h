#pragma once

#include "ViewModel.h"
#include <string>
#include <vector>
#include <unordered_map>

class MaterialsDataProvider;
class GeometryStorage;

class PrimitiveViewModel : public ViewModel
{

public:
	PrimitiveViewModel(
		MaterialsDataProvider& materialsDataProvider,
		GeometryStorage& geometryStorage);
	~PrimitiveViewModel() = default;

	void update();

	std::string mesh;
	std::string subMesh;
	std::string material;
	float position[3];
	float scaling[3];
	float texture[3];


	std::unordered_map<std::string, std::vector<std::string>> meshes;
	std::vector<std::string> materials;


private:
	MaterialsDataProvider& materialsDataProvider;
	GeometryStorage& geometryStorage;
};
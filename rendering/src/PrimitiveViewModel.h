#pragma once

#include "ViewModel.h"
#include <string>
#include <vector>
#include <unordered_map>

class MaterialsDataProvider;
class GeometryStorage;
class Scene;

class PrimitiveViewModel : public ViewModel
{

public:
	PrimitiveViewModel(
		Scene& scene,
		MaterialsDataProvider& materialsDataProvider,
		GeometryStorage& geometryStorage);
	~PrimitiveViewModel() = default;

	void update();

	void createPrimitive();

	std::string mesh;
	std::string subMesh;
	std::string material;
	float position[3] = { 0,0,0 };
	float scaling[3] = { 0,0,0 };
	float texture[3] = { 0,0,0 };


	std::unordered_map<std::string, std::vector<std::string>> meshes;
	std::vector<std::string> materials;


private:
	Scene& scene;
	MaterialsDataProvider& materialsDataProvider;
	GeometryStorage& geometryStorage;
};
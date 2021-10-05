#include <vector>
#include "GeometryStorage.h"

class MaterialsDataProvider;
struct MeshData;

#pragma once
class OBJFileLoader
{
public:
	OBJFileLoader(
		GeometryStorage& geometryStorage, 
		MaterialsDataProvider& materialsDataProvider
	);
	~OBJFileLoader() = default;
private:
	GeometryStorage& geometryStorage;
	MaterialsDataProvider& materialsDataProvider;

	std::vector<MeshData> objModelMeshes();
};


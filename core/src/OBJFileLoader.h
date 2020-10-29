#include <vector>
#include "GeometryStorage.h"

class MaterialsDataProvider;
class RenderItemTemplatesProvider;
struct MeshData;

#pragma once
class OBJFileLoader
{
public:
	OBJFileLoader(
		GeometryStorage& geometryStorage, 
		MaterialsDataProvider& materialsDataProvider,
		RenderItemTemplatesProvider& renderItemTemplatesProvider
	);
	~OBJFileLoader() = default;
private:
	GeometryStorage& geometryStorage;
	MaterialsDataProvider& materialsDataProvider;
	RenderItemTemplatesProvider& renderItemTemplatesProvider;

	std::vector<MeshData> objModelMeshes();
};


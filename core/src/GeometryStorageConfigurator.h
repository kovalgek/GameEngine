#include <vector>
#include <memory>
#include "GeometryGenerator.h"
//class GeometryGenerator;
class GeometryStorage;
struct MeshData;

#pragma once
class GeometryStorageConfigurator
{
public:
	GeometryStorageConfigurator(std::unique_ptr<GeometryGenerator> geometryGenerator);
	~GeometryStorageConfigurator() = default;

	void configure(GeometryStorage& geometryStorage);
private:
	std::vector<MeshData> primitiveShapeMeshes();
	std::vector<MeshData> customModelMeshes();

	std::unique_ptr<GeometryGenerator> geometryGenerator;
};


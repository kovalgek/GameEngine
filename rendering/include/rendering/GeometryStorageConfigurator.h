#pragma once

#include <vector>
#include <memory>

class GeometryStorage;
class DynamicVerticesProvider;
class GeometryGenerator;
struct MeshData;

class GeometryStorageConfigurator
{
public:
	GeometryStorageConfigurator(GeometryGenerator& geometryGenerator);
	~GeometryStorageConfigurator();

	void configure(GeometryStorage& geometryStorage);
	void configure(GeometryStorage& geometryStorage, DynamicVerticesProvider &dynamicVerticesProvider);
private:
	std::vector<MeshData> primitiveShapeMeshes();
	std::vector<MeshData> customModelMeshes();

	GeometryGenerator& geometryGenerator;
};


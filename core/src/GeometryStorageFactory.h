#include <memory>
#include <vector>
#include "GeometryGenerator.h"

class GeometryStorage;
//class GeometryGenerator;
//struct GeometryGenerator::MeshData;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;

#pragma once
class GeometryStorageFactory
{
public:

	GeometryStorageFactory(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		std::unique_ptr<GeometryGenerator> geoGen
	);
	~GeometryStorageFactory();

	std::unique_ptr<GeometryStorage> getGeometryStorage();

private:

	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;
	std::unique_ptr<GeometryGenerator> geoGen;

	std::vector<GeometryGenerator::MeshData> shapeGeoMeshes();
	std::vector<GeometryGenerator::MeshData> modelMeshes();
};


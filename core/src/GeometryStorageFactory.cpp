#include "GeometryStorageFactory.h"
#include "GeometryStorage.h"
#include <d3d12.h>

GeometryStorageFactory::GeometryStorageFactory(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList,
	std::unique_ptr<GeometryGenerator> geoGen
) : 
	device { device },
	commandList { commandList },
	geoGen { std::move(geoGen) }
{

}

GeometryStorageFactory::~GeometryStorageFactory() = default;

std::unique_ptr<GeometryStorage> GeometryStorageFactory::getGeometryStorage()
{
	auto geometryStorage = std::make_unique<GeometryStorage>(device, commandList);

	geometryStorage->createMeshGeometry("shapeGeo", GeometryStorageFactory::shapeGeoMeshes());
	geometryStorage->createMeshGeometry("models", GeometryStorageFactory::modelMeshes());

	return geometryStorage;
}

std::vector<GeometryGenerator::MeshData> GeometryStorageFactory::modelMeshes()
{
	std::vector<GeometryGenerator::MeshData> meshes;

	auto skullMesh = geoGen->createModel("../core/resources/models/skull.txt");
	skullMesh.Name = "skull";
	meshes.push_back(skullMesh);

	auto carMesh = geoGen->createModel("../core/resources/models/car.txt");
	carMesh.Name = "car";
	meshes.push_back(carMesh);

	return meshes;
}

std::vector<GeometryGenerator::MeshData> GeometryStorageFactory::shapeGeoMeshes()
{
	std::vector<GeometryGenerator::MeshData> meshes;

	GeometryGenerator::MeshData box = geoGen->CreateBox(8.0f, 8.0f, 8.0f, 3);
	box.Name = "box";
	meshes.push_back(box);

	GeometryGenerator::MeshData grid = geoGen->CreateGrid(20.0f, 30.0f, 60, 40);
	grid.Name = "grid";
	meshes.push_back(grid);

	GeometryGenerator::MeshData sphere = geoGen->CreateSphere(0.5f, 20, 20);
	sphere.Name = "sphere";
	meshes.push_back(sphere);

	GeometryGenerator::MeshData cylinder = geoGen->CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	cylinder.Name = "cylinder";
	meshes.push_back(cylinder);

	GeometryGenerator::MeshData hills = geoGen->CreateHills(160.0f, 160.0f, 50, 50);
	hills.Name = "hills";
	meshes.push_back(hills);

	return meshes;
}
#include "GeometryStorageConfigurator.h"

#include "GeometryStorage.h"
#include "MeshData.h"

#include "Waves.h"
#include "DynamicVerticesProvider.h"
#include "DynamicVertices.h"

GeometryStorageConfigurator::GeometryStorageConfigurator(std::unique_ptr<GeometryGenerator> geometryGenerator) : geometryGenerator { std::move(geometryGenerator) }
{

}

void GeometryStorageConfigurator::configure(GeometryStorage& geometryStorage)
{
	geometryStorage.createMeshGeometry("shapeGeo", primitiveShapeMeshes());
	geometryStorage.createMeshGeometry("models", customModelMeshes());
}

void GeometryStorageConfigurator::configure(GeometryStorage& geometryStorage, DynamicVerticesProvider& dynamicVerticesProvider)
{
	auto waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
	geometryStorage.createMeshGeometry("dynamic", "waves", waves->getIndices(), waves->VertexCount());
	dynamicVerticesProvider.addDynamicVertices("waves", std::move(waves));
}

std::vector<MeshData> GeometryStorageConfigurator::customModelMeshes()
{
	std::vector<MeshData> meshes;

	auto skullMesh = geometryGenerator->createModel("../core/resources/models/skull.txt");
	skullMesh.Name = "skull";
	meshes.push_back(skullMesh);

	auto carMesh = geometryGenerator->createModel("../core/resources/models/car.txt");
	carMesh.Name = "car";
	meshes.push_back(carMesh);

	return meshes;
}

std::vector<MeshData> GeometryStorageConfigurator::primitiveShapeMeshes()
{
	std::vector<MeshData> meshes;

	MeshData box = geometryGenerator->CreateBox(8.0f, 8.0f, 8.0f, 3);
	box.Name = "box";
	meshes.push_back(box);

	MeshData grid = geometryGenerator->CreateGrid(20.0f, 30.0f, 60, 40);
	grid.Name = "grid";
	meshes.push_back(grid);

	MeshData sphere = geometryGenerator->CreateSphere(0.5f, 20, 20);
	sphere.Name = "sphere";
	meshes.push_back(sphere);

	MeshData cylinder = geometryGenerator->CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	cylinder.Name = "cylinder";
	meshes.push_back(cylinder);

	MeshData hills = geometryGenerator->CreateHills(160.0f, 160.0f, 50, 50);
	hills.Name = "hills";
	meshes.push_back(hills);

	return meshes;
}
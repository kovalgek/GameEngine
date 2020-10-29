#include "OBJFileLoader.h"
#include "MeshData.h"

#include "MaterialsDataProvider.h"
#include "RenderItemTemplatesProvider.h"
#include "WaveFrontReader.h"

using namespace DirectX;

OBJFileLoader::OBJFileLoader(
    GeometryStorage& geometryStorage,
    MaterialsDataProvider& materialsDataProvider,
    RenderItemTemplatesProvider& renderItemTemplatesProvider
) : 
	geometryStorage { geometryStorage },
	materialsDataProvider{ materialsDataProvider },
	renderItemTemplatesProvider{ renderItemTemplatesProvider }
{
    geometryStorage.createMeshGeometry("obj", objModelMeshes());
}

std::vector<MeshData> OBJFileLoader::objModelMeshes()
{
	std::vector<MeshData> meshes;

	WaveFrontReader<uint32> waveFrontReader;
	waveFrontReader.Load(L"../core/resources/models/suzanne.obj");

	MeshData objMesh;
	objMesh.Name = "suzanne";

	for (const WaveFrontReader<uint32>::Vertex& vertex : waveFrontReader.vertices)
	{
		DirectX::XMFLOAT3 empty;
		MeshVertex meshVertex(vertex.position, vertex.normal, empty, vertex.textureCoordinate);
		objMesh.Vertices.push_back(meshVertex);
	}

	for (uint32 index : waveFrontReader.indices)
	{
		objMesh.Indices32.push_back(index);
	}

	meshes.push_back(objMesh);

	return meshes;
}
#include "PrimitiveViewModel.h"
#include "MaterialsDataProvider.h"
#include "GeometryStorage.h"
#include "Scene.h"

PrimitiveViewModel::PrimitiveViewModel(
	Scene& scene,
	MaterialsDataProvider& materialsDataProvider,
	GeometryStorage& geometryStorage) : 
	scene { scene },
	materialsDataProvider{ materialsDataProvider },
	geometryStorage{ geometryStorage }
{
	position[0] = 0.0f;
	position[1] = 0.0f;
	position[2] = 0.0f;

	scaling[0] = 1.0f;
	scaling[1] = 1.0f;
	scaling[2] = 1.0f;

	texture[0] = 1.0f;
	texture[1] = 1.0f;
	texture[2] = 1.0f;

	meshes = geometryStorage.getGeometryNames();
	if (meshes.size() != 0)
	{
		auto firstMesh = meshes.begin();
		mesh = firstMesh->first;

		auto submeshes = firstMesh->second;
		if (submeshes.size() != 0)
		{
			subMesh = submeshes[0];
		}
	}

	materials = materialsDataProvider.getMaterialNames();
	if (materials.size() != 0)
	{
		material = materials[0];
	}
}

void PrimitiveViewModel::createPrimitive()
{
	auto positionParameter = std::vector<float>(std::begin(position), std::end(position));
	auto scalingParameter = std::vector<float>(std::begin(scaling), std::end(scaling));
	auto textureParameter = std::vector<float>(std::begin(texture), std::end(texture));

	//scene.createRenderItem(
	//	mesh,
	//	subMesh,
	//	material,
	//	positionParameter,
	//	scalingParameter,
	//	textureParameter,
	//	{ RenderLayer::Opaque }
	//);
}

void PrimitiveViewModel::update()
{

}
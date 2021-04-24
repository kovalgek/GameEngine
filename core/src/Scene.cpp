#include "Scene.h"
#include "RenderItem.h"
#include "MeshGeometry.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"
#include <algorithm>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

Scene::Scene(
	const GeometryStorage& geometryStorage,
	const MaterialsDataProvider& materialsDataProvider
) : 
	geometryStorage{ geometryStorage },
	materialsDataProvider{ materialsDataProvider }
{
	sceneDidLoad();
}

Scene::~Scene() = default;

std::unique_ptr<RenderItem> Scene::createRenderItem(
	std::string meshName,
	std::string submeshName,
	std::string material,
	std::vector<float> position,
	std::vector<float> scaling,
	std::vector<float> textureTransform,
	RenderLayer2 renderLayer
) {
	auto item = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&item->World, XMMatrixScaling(scaling[0], scaling[1], scaling[2]) * XMMatrixTranslation(position[0], position[1], position[2]));
	XMStoreFloat4x4(&item->TexTransform, XMMatrixScaling(textureTransform[0], textureTransform[1], textureTransform[2]));

	item->ObjCBIndex = currentObjCBIndex();

	item->Mat = materialsDataProvider.getMaterialForName(material);
	item->Geo = geometryStorage.getGeometry(meshName);
	item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item->IndexCount = item->Geo->DrawArgs[submeshName].IndexCount;
	item->StartIndexLocation = item->Geo->DrawArgs[submeshName].StartIndexLocation;
	item->BaseVertexLocation = item->Geo->DrawArgs[submeshName].BaseVertexLocation;

	renderItemLayer[(int)RenderLayer2::Opaque].push_back(item.get());

	return item;
}

int Scene::currentObjCBIndex()
{
	return renderItems.size();
}

std::vector<RenderItem*> Scene::getRenderItems()
{
	std::vector<RenderItem*> renderItemsWithPointers(renderItems.size());

	std::transform(renderItems.begin(), renderItems.end(), renderItemsWithPointers.begin(), [](const std::unique_ptr<RenderItem> &renderItem) {
		return renderItem.get();
	});

	return renderItemsWithPointers;
}

std::vector<RenderItem*> Scene::getRenderItemsForLayer(RenderLayer2 layer)
{
	return renderItemLayer[(int)layer];
}

void Scene::sceneDidLoad()
{
	auto item = createRenderItem(
		"shapeGeo",
		"box",
		"grass",
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 5.0f,5.0f,5.0f }
	);

	addItem(std::move(item), RenderLayer2::Opaque);
}

void Scene::addItem(std::unique_ptr<RenderItem> renderItem, RenderLayer2 renderLayer)
{
	renderItemLayer[(int)renderLayer].push_back(renderItem.get());
	renderItems.push_back(std::move(renderItem));
}
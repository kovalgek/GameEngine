#include "ObjectsDataProvider.h"
#include <DirectXPackedVector.h>
#include "RenderItem.h"
#include "GeometryStorage.h"
#include "d3dUtil.h"
#include "MaterialsDataProvider.h"
#include "DynamicVerticesProvider.h"
#include "RenderItemTemplate.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

ObjectsDataProvider::ObjectsDataProvider(
	std::unique_ptr <GeometryStorage> geometryStorage,
	const MaterialsDataProvider& materialsDataProvider,
	const DynamicVerticesProvider& dynamicVerticesProvider
) :
	geometryStorage { std::move(geometryStorage) },
	materialsDataProvider { materialsDataProvider },
	dynamicVerticesProvider { dynamicVerticesProvider }
{

}

ObjectsDataProvider::~ObjectsDataProvider() = default;

void ObjectsDataProvider::createPrimitive(
	std::string meshName,
	std::string subMeshName,
	std::string material,
	std::vector<float> position,
	std::vector<float> scaling,
	std::vector<float> textureTransform)
{
	auto item = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&item->World, XMMatrixScaling(scaling[0], scaling[1], scaling[2]) * XMMatrixTranslation(position[0], position[1], position[2]));
	XMStoreFloat4x4(&item->TexTransform, XMMatrixScaling(textureTransform[0], textureTransform[1], textureTransform[2]));

	item->ObjCBIndex = itemIndex++;

	item->Mat = materialsDataProvider.getMaterialForName(material);
	item->Geo = this->geometryStorage->getGeometry(meshName);
	item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item->IndexCount = item->Geo->DrawArgs[subMeshName].IndexCount;
	item->StartIndexLocation = item->Geo->DrawArgs[subMeshName].StartIndexLocation;
	item->BaseVertexLocation = item->Geo->DrawArgs[subMeshName].BaseVertexLocation;

	ritemLayer[(int)RenderLayer::Opaque].push_back(item.get());
	allRitems.push_back(std::move(item));
}

void ObjectsDataProvider::createPrimitive(PrimitiveModel primitiveModel)
{
	auto item = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&item->World, XMMatrixScaling(primitiveModel.scaling[0], primitiveModel.scaling[1], primitiveModel.scaling[2]) * XMMatrixTranslation(primitiveModel.position[0], primitiveModel.position[1], primitiveModel.position[2]));
	XMStoreFloat4x4(&item->TexTransform, XMMatrixScaling(primitiveModel.texture[0], primitiveModel.texture[1], primitiveModel.texture[2]));

	item->ObjCBIndex = itemIndex++;
	item->Mat = materialsDataProvider.getMaterialForName(primitiveModel.material);
	item->Geo = this->geometryStorage->getGeometry(primitiveModel.mesh);
	item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item->IndexCount = item->Geo->DrawArgs[primitiveModel.submesh].IndexCount;
	item->StartIndexLocation = item->Geo->DrawArgs[primitiveModel.submesh].StartIndexLocation;
	item->BaseVertexLocation = item->Geo->DrawArgs[primitiveModel.submesh].BaseVertexLocation;

	ritemLayer[(int)RenderLayer::Opaque].push_back(item.get());
	allRitems.push_back(std::move(item));
}

void ObjectsDataProvider::createRenderItem(
	RenderItemTemplate renderItemTemplate,
	std::vector<float> position,
	std::vector<float> scaling,
	std::vector<float> textureTransform,
	bool dynamicVertices,
	RenderLayer renderLayer)
{
	auto item = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&item->World, XMMatrixScaling(scaling[0], scaling[1], scaling[2]) * XMMatrixTranslation(position[0], position[1], position[2]));
	XMStoreFloat4x4(&item->TexTransform, XMMatrixScaling(textureTransform[0], textureTransform[1], textureTransform[2]));

	item->ObjCBIndex = itemIndex++;

	item->Mat = materialsDataProvider.getMaterialForName(renderItemTemplate.material);
	item->Geo = this->geometryStorage->getGeometry(renderItemTemplate.mesh);
	item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item->IndexCount = item->Geo->DrawArgs[renderItemTemplate.submesh].IndexCount;
	item->StartIndexLocation = item->Geo->DrawArgs[renderItemTemplate.submesh].StartIndexLocation;
	item->BaseVertexLocation = item->Geo->DrawArgs[renderItemTemplate.submesh].BaseVertexLocation;

	if (dynamicVertices)
	{
		item->dynamicVertices = dynamicVerticesProvider.getDynamicVerticesForName("waves");
	}

	ritemLayer[(int)renderLayer].push_back(item.get());
	allRitems.push_back(std::move(item));
}

std::vector<RenderItem*> ObjectsDataProvider::renderItems()
{
	std::vector<RenderItem*> temp;
	for (auto& e : allRitems)
	{
		temp.push_back(e.get());
	}
	return temp;
}

std::vector<RenderItem*> ObjectsDataProvider::renderItemsForLayer(RenderLayer layer)
{
	return ritemLayer[(int)layer];
}

std::vector<RenderItem*> ObjectsDataProvider::getRenderItemsWithDynamicVertexBuffer()
{
	std::vector<RenderItem*> itemsWithDynamicVertexBuffer;

	for (auto& e : allRitems)
	{
		if (e.get()->dynamicVertices)
		{
			itemsWithDynamicVertexBuffer.push_back(e.get());
		}
	}
	return itemsWithDynamicVertexBuffer;
}
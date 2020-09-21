#include "ObjectsDataProvider.h"
#include <DirectXPackedVector.h>
#include "RenderItem.h"
#include "GeometryStorage.h"
#include "d3dUtil.h"
#include "MaterialsDataProvider.h"
#include "DynamicVerticesProvider.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

ObjectsDataProvider::ObjectsDataProvider(
	std::unique_ptr <GeometryStorage> geometryStorage,
	MaterialsDataProvider* materialsDataProvider,
	DynamicVerticesProvider* dynamicVerticesProvider
) :
	geometryStorage { std::move(geometryStorage) },
	materialsDataProvider { materialsDataProvider },
	dynamicVerticesProvider { dynamicVerticesProvider }
{
	buildRenderItemsForLandAndWaves();

	//createPrimitive("hills", "grass", { 0.0f, 0.0f, 0.0f, }, { 1.0f, 1.0f, 1.0f, }, { 5.0f, 5.0f, 1.0f, });
	//createPrimitive("box", "wirefence", { 0.0f, 0.0f, 0.0f, }, { 1.0f, 1.0f, 1.0f, }, { 5.0f, 5.0f, 1.0f, });
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
	item->Mat = materialsDataProvider->getMaterialForName(material);
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
	item->Mat = materialsDataProvider->getMaterialForName(primitiveModel.material);
	item->Geo = this->geometryStorage->getGeometry(primitiveModel.mesh);
	item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item->IndexCount = item->Geo->DrawArgs[primitiveModel.submesh].IndexCount;
	item->StartIndexLocation = item->Geo->DrawArgs[primitiveModel.submesh].StartIndexLocation;
	item->BaseVertexLocation = item->Geo->DrawArgs[primitiveModel.submesh].BaseVertexLocation;

	ritemLayer[(int)RenderLayer::Opaque].push_back(item.get());
	allRitems.push_back(std::move(item));
}

void ObjectsDataProvider::buildRenderItemsForShapes()
{
	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->ObjCBIndex = 0;
	boxRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	allRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = MathHelper::Identity4x4();
	gridRitem->ObjCBIndex = 1;
	gridRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	allRitems.push_back(std::move(gridRitem));

	UINT objCBIndex = 2;
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
		rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		allRitems.push_back(std::move(leftCylRitem));
		allRitems.push_back(std::move(rightCylRitem));
		allRitems.push_back(std::move(leftSphereRitem));
		allRitems.push_back(std::move(rightSphereRitem));
	}
}

void ObjectsDataProvider::buildRenderItemsForLandAndWaves()
{
	auto wavesRitem = std::make_unique<RenderItem>();
	wavesRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&wavesRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	wavesRitem->ObjCBIndex = 0;
	wavesRitem->Mat = materialsDataProvider->getMaterialForName("water");
	wavesRitem->Geo = this->geometryStorage->getGeometry("waterGeo");
	wavesRitem->dynamicVertices = dynamicVerticesProvider->getDynamicVerticesForName("waves");
	wavesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	renderItemsWithDynamicVertexBuffer.push_back(wavesRitem.get());

	ritemLayer[(int)RenderLayer::Transparent].push_back(wavesRitem.get());

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	gridRitem->ObjCBIndex = 1;
	gridRitem->Mat = materialsDataProvider->getMaterialForName("grass");
	gridRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["hills"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["hills"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["hills"].BaseVertexLocation;

	ritemLayer[(int)RenderLayer::Opaque].push_back(gridRitem.get());

	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixTranslation(3.0f, 2.0f, -9.0f));
	boxRitem->ObjCBIndex = 2;
	boxRitem->Mat = materialsDataProvider->getMaterialForName("wirefence");
	boxRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	ritemLayer[(int)RenderLayer::Opaque].push_back(boxRitem.get());

	auto skullRitem = std::make_unique<RenderItem>();
	skullRitem->World = MathHelper::Identity4x4();
	skullRitem->TexTransform = MathHelper::Identity4x4();
	skullRitem->ObjCBIndex = 0;
	skullRitem->Mat = materialsDataProvider->getMaterialForName("skullMat");// mMaterials["skullMat"].get();
	skullRitem->Geo = this->geometryStorage->getGeometry("skullGeo");// mGeometries["skullGeo"].get();
	skullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skullRitem->IndexCount = skullRitem->Geo->DrawArgs["skull"].IndexCount;
	skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
	skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
	//mSkullRitem = skullRitem.get();
	//ritemLayer[(int)RenderLayer::Opaque].push_back(skullRitem.get());

	allRitems.push_back(std::move(wavesRitem));
	allRitems.push_back(std::move(gridRitem));
	allRitems.push_back(std::move(boxRitem));
	//allRitems.push_back(std::move(skullRitem));
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
#include "ObjectsDataProvider.h"
#include <DirectXPackedVector.h>
#include "RenderItem.h"
#include "GeometryStorage.h"
#include "d3dUtil.h"
#include "MaterialsDataProvider.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

ObjectsDataProvider::ObjectsDataProvider(
	std::unique_ptr <GeometryStorage> geometryStorage,
	MaterialsDataProvider* materialsDataProvider
) :
	geometryStorage { std::move(geometryStorage) },
	materialsDataProvider { materialsDataProvider }
{
	//buildRenderItemsForLandAndWaves();

	//createPrimitive("hills", "grass", { 0.0f, 0.0f, 0.0f, }, { 1.0f, 1.0f, 1.0f, }, { 5.0f, 5.0f, 1.0f, });
	//createPrimitive("box", "wirefence", { 0.0f, 0.0f, 0.0f, }, { 1.0f, 1.0f, 1.0f, }, { 5.0f, 5.0f, 1.0f, });
}

ObjectsDataProvider::~ObjectsDataProvider() = default;

void ObjectsDataProvider::createPrimitive(
	std::string name,
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
	item->Geo = this->geometryStorage->getGeometry("shapeGeo");
	item->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	item->IndexCount = item->Geo->DrawArgs[name].IndexCount;
	item->StartIndexLocation = item->Geo->DrawArgs[name].StartIndexLocation;
	item->BaseVertexLocation = item->Geo->DrawArgs[name].BaseVertexLocation;

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
	//auto wavesRitem = std::make_unique<RenderItem>();
	//wavesRitem->World = MathHelper::Identity4x4();
	//XMStoreFloat4x4(&wavesRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	//wavesRitem->ObjCBIndex = 0;
	//wavesRitem->Mat = materialsDataProvider->getMaterialForName("water");
	//wavesRitem->Geo = this->geometryStorage->getGeometry("waterGeo");
	//wavesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	//wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	//wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	//mWavesRitem = wavesRitem.get();

	//ritemLayer[(int)RenderLayer::Opaque].push_back(wavesRitem.get());

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	gridRitem->ObjCBIndex = 0;
	gridRitem->Mat = materialsDataProvider->getMaterialForName("grass");
	gridRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	ritemLayer[(int)RenderLayer::Opaque].push_back(gridRitem.get());

	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixTranslation(3.0f, 2.0f, -9.0f));
	boxRitem->ObjCBIndex = 1;
	boxRitem->Mat = materialsDataProvider->getMaterialForName("grass");
	boxRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;

	ritemLayer[(int)RenderLayer::Opaque].push_back(boxRitem.get());

	//allRitems.push_back(std::move(wavesRitem));
	allRitems.push_back(std::move(gridRitem));
	allRitems.push_back(std::move(boxRitem));
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
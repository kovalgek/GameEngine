#include "ObjectsDataProviderConfigurator.h"
#include "RenderItem.h"
#include <memory>
#include <DirectXMath.h>
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"
#include "DynamicVerticesProvider.h"
#include "ObjectsDataProvider.h"
#include "RenderItemTemplatesProvider.h"
#include "RenderItemTemplate.h"

using namespace DirectX;

ObjectsDataProviderConfigurator::ObjectsDataProviderConfigurator(const RenderItemTemplatesProvider& renderItemTemplatesProvider) : renderItemTemplatesProvider { renderItemTemplatesProvider }
{
}

void ObjectsDataProviderConfigurator::configure(ObjectsDataProvider& objectsDataProvider)
{
	buildRenderItemsForShapes(objectsDataProvider);
}

void ObjectsDataProviderConfigurator::buildRenderItemsForShapes(ObjectsDataProvider& objectsDataProvider)
{
	//objectsDataProvider.createPrimitive(
	//	"shapeGeo",
	//	"box",
	//	"water",
	//	{ 0.0f,0.0f,0.0f },
	//	{ 1.0f,1.0f,1.0f },
	//	{ 0.0f,0.0f,0.0f }
	//);

	auto boxTemplate = renderItemTemplatesProvider.getRenderItemTemplate("BoxWithWaterMaterial");
	objectsDataProvider.createRenderItem(
		*boxTemplate, 
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f }
	);

	//auto boxRitem = std::make_unique<RenderItem>();
	//XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	//boxRitem->ObjCBIndex = 0;
	//boxRitem->Geo = geometryStorage.getGeometry("shapeGeo");
	//boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	//boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	//boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	//allRitems.push_back(std::move(boxRitem));

	//auto gridRitem = std::make_unique<RenderItem>();
	//gridRitem->World = MathHelper::Identity4x4();
	//gridRitem->ObjCBIndex = 1;
	//gridRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	//gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	//gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	//gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	//allRitems.push_back(std::move(gridRitem));

	//UINT objCBIndex = 2;
	//for (int i = 0; i < 5; ++i)
	//{
	//	auto leftCylRitem = std::make_unique<RenderItem>();
	//	auto rightCylRitem = std::make_unique<RenderItem>();
	//	auto leftSphereRitem = std::make_unique<RenderItem>();
	//	auto rightSphereRitem = std::make_unique<RenderItem>();

	//	XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
	//	XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

	//	XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
	//	XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

	//	XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
	//	leftCylRitem->ObjCBIndex = objCBIndex++;
	//	leftCylRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	//	leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
	//	leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
	//	leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

	//	XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
	//	rightCylRitem->ObjCBIndex = objCBIndex++;
	//	rightCylRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	//	rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
	//	rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
	//	rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

	//	XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
	//	leftSphereRitem->ObjCBIndex = objCBIndex++;
	//	leftSphereRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	//	leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
	//	leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	//	leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	//	XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
	//	rightSphereRitem->ObjCBIndex = objCBIndex++;
	//	rightSphereRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
	//	rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
	//	rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	//	rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	//	allRitems.push_back(std::move(leftCylRitem));
	//	allRitems.push_back(std::move(rightCylRitem));
	//	allRitems.push_back(std::move(leftSphereRitem));
	//	allRitems.push_back(std::move(rightSphereRitem));
	//}
}

//void ObjectsDataProviderConfigurator::buildRenderItemsForLandAndWaves()
//{
//	auto wavesRitem = std::make_unique<RenderItem>();
//	wavesRitem->World = MathHelper::Identity4x4();
//	XMStoreFloat4x4(&wavesRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
//	wavesRitem->ObjCBIndex = 0;
//	wavesRitem->Mat = materialsDataProvider.getMaterialForName("water");
//	wavesRitem->Geo = this->geometryStorage->getGeometry("waterGeo");
//	wavesRitem->dynamicVertices = dynamicVerticesProvider.getDynamicVerticesForName("waves");
//	wavesRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
//	wavesRitem->StartIndexLocation = wavesRitem->Geo->DrawArgs["grid"].StartIndexLocation;
//	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
//
//	renderItemsWithDynamicVertexBuffer.push_back(wavesRitem.get());
//
//	ritemLayer[(int)RenderLayer::Transparent].push_back(wavesRitem.get());
//
//	auto gridRitem = std::make_unique<RenderItem>();
//	gridRitem->World = MathHelper::Identity4x4();
//	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
//	gridRitem->ObjCBIndex = 1;
//	gridRitem->Mat = materialsDataProvider.getMaterialForName("grass");
//	gridRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
//	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["hills"].IndexCount;
//	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["hills"].StartIndexLocation;
//	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["hills"].BaseVertexLocation;
//
//	ritemLayer[(int)RenderLayer::Opaque].push_back(gridRitem.get());
//
//	auto boxRitem = std::make_unique<RenderItem>();
//	XMStoreFloat4x4(&boxRitem->World, XMMatrixTranslation(3.0f, 2.0f, -9.0f));
//	boxRitem->ObjCBIndex = 2;
//	boxRitem->Mat = materialsDataProvider.getMaterialForName("wirefence");
//	boxRitem->Geo = this->geometryStorage->getGeometry("shapeGeo");
//	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
//	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
//	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
//
//	//ritemLayer[(int)RenderLayer::Opaque].push_back(boxRitem.get());
//
//	auto skullRitem = std::make_unique<RenderItem>();
//	skullRitem->World = MathHelper::Identity4x4();
//	skullRitem->TexTransform = MathHelper::Identity4x4();
//	skullRitem->ObjCBIndex = 0;
//	skullRitem->Mat = materialsDataProvider.getMaterialForName("skullMat");
//	skullRitem->Geo = this->geometryStorage->getGeometry("skullGeo");
//	skullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	skullRitem->IndexCount = skullRitem->Geo->DrawArgs["skull"].IndexCount;
//	skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
//	skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
//	//mSkullRitem = skullRitem.get();
//	ritemLayer[(int)RenderLayer::Opaque].push_back(skullRitem.get());
//
//	allRitems.push_back(std::move(wavesRitem));
//	allRitems.push_back(std::move(gridRitem));
//	//allRitems.push_back(std::move(boxRitem));
//	allRitems.push_back(std::move(skullRitem));
//}
//
//void ObjectsDataProviderConfigurator::buildStencilDemoRenderItems()
//{
//	auto floorRitem = std::make_unique<RenderItem>();
//	floorRitem->World = MathHelper::Identity4x4();
//	floorRitem->TexTransform = MathHelper::Identity4x4();
//	floorRitem->ObjCBIndex = 0;
//	floorRitem->Mat = materialsDataProvider.getMaterialForName("checkertile");
//	floorRitem->Geo = this->geometryStorage->getGeometry("roomGeo");
//	floorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	floorRitem->IndexCount = floorRitem->Geo->DrawArgs["floor"].IndexCount;
//	floorRitem->StartIndexLocation = floorRitem->Geo->DrawArgs["floor"].StartIndexLocation;
//	floorRitem->BaseVertexLocation = floorRitem->Geo->DrawArgs["floor"].BaseVertexLocation;
//	ritemLayer[(int)RenderLayer::Opaque].push_back(floorRitem.get());
//
//	auto wallsRitem = std::make_unique<RenderItem>();
//	wallsRitem->World = MathHelper::Identity4x4();
//	wallsRitem->TexTransform = MathHelper::Identity4x4();
//	wallsRitem->ObjCBIndex = 1;
//	wallsRitem->Mat = materialsDataProvider.getMaterialForName("bricks");
//	wallsRitem->Geo = this->geometryStorage->getGeometry("roomGeo");
//	wallsRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	wallsRitem->IndexCount = wallsRitem->Geo->DrawArgs["wall"].IndexCount;
//	wallsRitem->StartIndexLocation = wallsRitem->Geo->DrawArgs["wall"].StartIndexLocation;
//	wallsRitem->BaseVertexLocation = wallsRitem->Geo->DrawArgs["wall"].BaseVertexLocation;
//	ritemLayer[(int)RenderLayer::Opaque].push_back(wallsRitem.get());
//
//	auto skullRitem = std::make_unique<RenderItem>();
//	skullRitem->World = MathHelper::Identity4x4();
//	skullRitem->TexTransform = MathHelper::Identity4x4();
//	skullRitem->ObjCBIndex = 2;
//	skullRitem->Mat = materialsDataProvider.getMaterialForName("skullMat");
//	skullRitem->Geo = this->geometryStorage->getGeometry("skullGeo");
//	skullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	skullRitem->IndexCount = skullRitem->Geo->DrawArgs["skull"].IndexCount;
//	skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
//	skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
//	//mSkullRitem = skullRitem.get();
//	ritemLayer[(int)RenderLayer::Opaque].push_back(skullRitem.get());
//
//	// Reflected skull will have different world matrix, so it needs to be its own render item.
//	auto reflectedSkullRitem = std::make_unique<RenderItem>();
//	*reflectedSkullRitem = *skullRitem;
//	reflectedSkullRitem->ObjCBIndex = 3;
//	//mReflectedSkullRitem = reflectedSkullRitem.get();
//	ritemLayer[(int)RenderLayer::Reflected].push_back(reflectedSkullRitem.get());
//
//	// Shadowed skull will have different world matrix, so it needs to be its own render item.
//	auto shadowedSkullRitem = std::make_unique<RenderItem>();
//	*shadowedSkullRitem = *skullRitem;
//	shadowedSkullRitem->ObjCBIndex = 4;
//	shadowedSkullRitem->Mat = materialsDataProvider.getMaterialForName("shadowMat");
//	//mShadowedSkullRitem = shadowedSkullRitem.get();
//	ritemLayer[(int)RenderLayer::Shadow].push_back(shadowedSkullRitem.get());
//
//	auto mirrorRitem = std::make_unique<RenderItem>();
//	mirrorRitem->World = MathHelper::Identity4x4();
//	mirrorRitem->TexTransform = MathHelper::Identity4x4();
//	mirrorRitem->ObjCBIndex = 5;
//	mirrorRitem->Mat = materialsDataProvider.getMaterialForName("icemirror");
//	mirrorRitem->Geo = this->geometryStorage->getGeometry("roomGeo");
//	mirrorRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	mirrorRitem->IndexCount = mirrorRitem->Geo->DrawArgs["mirror"].IndexCount;
//	mirrorRitem->StartIndexLocation = mirrorRitem->Geo->DrawArgs["mirror"].StartIndexLocation;
//	mirrorRitem->BaseVertexLocation = mirrorRitem->Geo->DrawArgs["mirror"].BaseVertexLocation;
//	ritemLayer[(int)RenderLayer::Mirrors].push_back(mirrorRitem.get());
//	ritemLayer[(int)RenderLayer::Transparent].push_back(mirrorRitem.get());
//
//	allRitems.push_back(std::move(floorRitem));
//	allRitems.push_back(std::move(wallsRitem));
//	allRitems.push_back(std::move(skullRitem));
//	allRitems.push_back(std::move(reflectedSkullRitem));
//	allRitems.push_back(std::move(shadowedSkullRitem));
//	allRitems.push_back(std::move(mirrorRitem));
//}
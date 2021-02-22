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
	//buildRenderItemsForShapes(objectsDataProvider);
	//buildRenderItemsForLandAndWaves(objectsDataProvider);
	buildRenderItemsForStencil(objectsDataProvider);
}

void ObjectsDataProviderConfigurator::buildRenderItemsForShapes(ObjectsDataProvider& objectsDataProvider)
{
	auto boxTemplate = renderItemTemplatesProvider.getRenderItemTemplate("BoxWithWaterMaterial");
	objectsDataProvider.createRenderItem(
		*boxTemplate, 
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		false,
		RenderLayer::AlphaTested
	);
}

void ObjectsDataProviderConfigurator::buildRenderItemsForLandAndWaves(ObjectsDataProvider& objectsDataProvider)
{
	auto boxTemplate = renderItemTemplatesProvider.getRenderItemTemplate("BoxWithWaterMaterial");
	objectsDataProvider.createRenderItem(
		*boxTemplate,
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		false,
		RenderLayer::AlphaTested
	);

	auto hillsTemplate = renderItemTemplatesProvider.getRenderItemTemplate("Hills");
	objectsDataProvider.createRenderItem(
		*hillsTemplate,
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 5.0f,5.0f,5.0f },
		false
	);

	auto waterWithWavesTemplate = renderItemTemplatesProvider.getRenderItemTemplate("WaterWithWaves");
	objectsDataProvider.createRenderItem(
		*waterWithWavesTemplate,
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 5.0f,5.0f,5.0f },
		true,
		RenderLayer::Transparent
	);
}

void ObjectsDataProviderConfigurator::buildRenderItemsForStencil(ObjectsDataProvider& objectsDataProvider)
{
	auto skullTemplate = renderItemTemplatesProvider.getRenderItemTemplate("Skull");
	objectsDataProvider.createRenderItem(
		*skullTemplate,
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 5.0f,5.0f,5.0f },
		false
	);

	auto reflectedSkullTemplate = renderItemTemplatesProvider.getRenderItemTemplate("Skull");
	objectsDataProvider.createRenderItem(
		*reflectedSkullTemplate,
		{ 0.0f,-10.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 5.0f,5.0f,5.0f },
		false,
		RenderLayer::Reflected
	);

	auto mirrorTemplate = renderItemTemplatesProvider.getRenderItemTemplate("Mirror");
	objectsDataProvider.createRenderItem(
		*mirrorTemplate,
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		false,
		RenderLayer::Mirrors
	);

	auto mirrorTemplate2 = renderItemTemplatesProvider.getRenderItemTemplate("Mirror");
	objectsDataProvider.createRenderItem(
		*mirrorTemplate2,
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		false,
		RenderLayer::Transparent
	);
}
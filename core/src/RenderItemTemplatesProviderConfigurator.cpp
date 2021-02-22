#include "RenderItemTemplatesProviderConfigurator.h"
#include "RenderItemTemplatesProvider.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"
#include "RenderItemTemplate.h"
#include "d3dUtil.h"

void RenderItemTemplatesProviderConfigurator::configure(RenderItemTemplatesProvider& renderItemTemplatesProvider)
{
	renderItemTemplatesProvider.addRenderItemTemplate("BoxWithWaterMaterial", boxWithWaterMaterial());
	renderItemTemplatesProvider.addRenderItemTemplate("Hills", hills());
	renderItemTemplatesProvider.addRenderItemTemplate("WaterWithWaves", waterWithWaves());
	renderItemTemplatesProvider.addRenderItemTemplate("Mirror", mirror());
	renderItemTemplatesProvider.addRenderItemTemplate("Skull", skull());
}

std::unique_ptr<RenderItemTemplate> RenderItemTemplatesProviderConfigurator::boxWithWaterMaterial()
{
	auto item = std::make_unique<RenderItemTemplate>();

	item->mesh = "shapeGeo";
	item->submesh = "box";
	item->material = "wirefence";

	return item;
}

std::unique_ptr<RenderItemTemplate> RenderItemTemplatesProviderConfigurator::hills()
{
	auto item = std::make_unique<RenderItemTemplate>();

	item->mesh = "shapeGeo";
	item->submesh = "hills";
	item->material = "grass";

	return item;
}

std::unique_ptr<RenderItemTemplate> RenderItemTemplatesProviderConfigurator::waterWithWaves()
{
	auto item = std::make_unique<RenderItemTemplate>();

	item->mesh = "dynamic";
	item->submesh = "waves";
	item->material = "water";

	return item;
}

std::unique_ptr<RenderItemTemplate> RenderItemTemplatesProviderConfigurator::mirror()
{
	auto item = std::make_unique<RenderItemTemplate>();

	item->mesh = "shapeGeo";
	item->submesh = "grid";
	item->material = "icemirror";

	return item;
}

std::unique_ptr<RenderItemTemplate> RenderItemTemplatesProviderConfigurator::skull()
{
	auto item = std::make_unique<RenderItemTemplate>();

	item->mesh = "models";
	item->submesh = "skull";
	item->material = "skullMat";

	return item;
}
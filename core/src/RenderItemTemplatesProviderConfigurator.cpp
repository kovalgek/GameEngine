#include "RenderItemTemplatesProviderConfigurator.h"
#include "RenderItemTemplatesProvider.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"
#include "RenderItemTemplate.h"
#include "d3dUtil.h"

void RenderItemTemplatesProviderConfigurator::configure(RenderItemTemplatesProvider& renderItemTemplatesProvider)
{
	renderItemTemplatesProvider.addRenderItemTemplate("BoxWithWaterMaterial", boxWithWaterMaterial());
}

std::unique_ptr<RenderItemTemplate> RenderItemTemplatesProviderConfigurator::boxWithWaterMaterial()
{
	auto item = std::make_unique<RenderItemTemplate>();

	item->mesh = "shapeGeo";
	item->submesh = "box";
	item->material = "grass";

	return item;
}
#include "RenderItemTemplatesProvider.h"
#include "RenderItemTemplate.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"
#include "d3dUtil.h"

RenderItemTemplatesProvider::RenderItemTemplatesProvider() = default;
RenderItemTemplatesProvider::~RenderItemTemplatesProvider() = default;

void RenderItemTemplatesProvider::addRenderItemTemplate(std::string name, std::unique_ptr<RenderItemTemplate> renderItemTemplate)
{
	renderItemTemplates[name] = std::move(renderItemTemplate);
}

RenderItemTemplate* RenderItemTemplatesProvider::getRenderItemTemplate(std::string name) const
{
	return renderItemTemplates.find(name)->second.get();
}
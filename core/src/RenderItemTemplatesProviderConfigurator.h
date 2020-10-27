#include <memory>

class RenderItemTemplatesProvider;
struct RenderItemTemplate;

#pragma once
class RenderItemTemplatesProviderConfigurator
{
public:
	RenderItemTemplatesProviderConfigurator() = default;
	~RenderItemTemplatesProviderConfigurator() = default;

	void configure(RenderItemTemplatesProvider &renderItemTemplatesProvider);
private:
	std::unique_ptr<RenderItemTemplate> boxWithWaterMaterial();
};


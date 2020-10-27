#include <string>
#include <memory>
#include <unordered_map>

struct RenderItemTemplate;

#pragma once
class RenderItemTemplatesProvider
{
public:
	RenderItemTemplatesProvider();
	~RenderItemTemplatesProvider();

	void addRenderItemTemplate(std::string name, std::unique_ptr<RenderItemTemplate> renderItemTemplate);
	RenderItemTemplate *getRenderItemTemplate(std::string name) const;
private:

	std::unordered_map<std::string, std::unique_ptr<RenderItemTemplate>> renderItemTemplates;
};


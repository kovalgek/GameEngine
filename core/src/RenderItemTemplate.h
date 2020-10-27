#include <string>

#pragma once
struct RenderItemTemplate
{
	RenderItemTemplate() = default;
	~RenderItemTemplate() = default;

	std::string mesh;
	std::string submesh;
	std::string material;
};


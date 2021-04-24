#pragma once

#include <vector>
#include <string>
#include <memory>

struct RenderItem;
class GeometryStorage;
class MaterialsDataProvider;

enum class RenderLayer2 : int
{
	Opaque = 0,
	Mirrors,
	Reflected,
	Transparent,
	Shadow,
	AlphaTested,
	Count
};

class Scene
{
public:

	Scene(
		const GeometryStorage& geometryStorage,
		const MaterialsDataProvider& materialsDataProvider
	);
	~Scene();

	std::vector<RenderItem*> getRenderItems();
	std::vector<RenderItem*> getRenderItemsForLayer(RenderLayer2 layer);

private:

	const GeometryStorage& geometryStorage;
	const MaterialsDataProvider& materialsDataProvider;

	std::unique_ptr<RenderItem> createRenderItem(
		std::string meshName,
		std::string submeshName,
		std::string material,
		std::vector<float> position,
		std::vector<float> scaling,
		std::vector<float> textureTransform,
		RenderLayer2 renderLayer = RenderLayer2::Opaque
	);

	std::vector<std::unique_ptr<RenderItem>> renderItems;
	std::vector<RenderItem*> renderItemLayer[(int)RenderLayer2::Count];

	int currentObjCBIndex();

	void sceneDidLoad();

	void addItem(std::unique_ptr<RenderItem> renderItem, RenderLayer2 renderLayer);
};
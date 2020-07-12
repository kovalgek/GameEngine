#include <vector>
#include <memory>
#include <unordered_map>

struct RenderItem;
class GeometryStorage;
class MaterialsDataProvider;

enum class RenderLayer : int
{
	Opaque = 0,
	Count
};

#pragma once
class ObjectsDataProvider
{
public:
	ObjectsDataProvider(
		std::unique_ptr <GeometryStorage> geometryStorage, 
		MaterialsDataProvider* materialsDataProvider
	);
	~ObjectsDataProvider();

	void onMouseDown(int x, int y);
	std::vector<RenderItem*> renderItems();
	std::vector<RenderItem*> opaqueRitems();

	std::vector<RenderItem*> renderItemsForLayer(RenderLayer layer);
	RenderItem* getWavesRitem() { return mWavesRitem; }

private:
	void buildRenderItemsForShapes();
	void buildRenderItemsForLandAndWaves();

	std::unique_ptr <GeometryStorage> geometryStorage;
	MaterialsDataProvider* materialsDataProvider;

	std::vector<std::shared_ptr<RenderItem>> allRitems;
	std::vector<RenderItem*> mOpaqueRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> ritemLayer[(int)RenderLayer::Count];
	RenderItem* mWavesRitem = nullptr;

	float delta;
};

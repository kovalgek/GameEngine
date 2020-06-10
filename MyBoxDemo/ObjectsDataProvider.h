#include <vector>
#include <memory>
#include <unordered_map>

struct RenderItem;
struct Material;
class GeometryStorage;

enum class RenderLayer : int
{
	Opaque = 0,
	Count
};

#pragma once
class ObjectsDataProvider
{
public:
	ObjectsDataProvider(std::unique_ptr <GeometryStorage> geometryStorage);
	~ObjectsDataProvider();

	void onMouseDown(int x, int y);
	std::vector<RenderItem*> renderItems();
	std::vector<RenderItem*> opaqueRitems();

	std::vector<RenderItem*> renderItemsForLayer(RenderLayer layer);
	RenderItem* getWavesRitem() { return mWavesRitem; }

	std::vector <Material*> getMaterials();
private:
	void buildRenderItemsForShapes();
	void buildRenderItemsForLandAndWaves();
	void buildMaterials();

	std::unique_ptr <GeometryStorage> geometryStorage;
	std::vector<std::shared_ptr<RenderItem>> allRitems;
	std::vector<RenderItem*> mOpaqueRitems;

	std::unordered_map<std::string, std::unique_ptr<Material>> materials;

	// Render items divided by PSO.
	std::vector<RenderItem*> ritemLayer[(int)RenderLayer::Count];
	RenderItem* mWavesRitem = nullptr;

	float delta;
};


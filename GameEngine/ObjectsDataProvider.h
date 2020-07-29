#include <vector>
#include <memory>
#include <unordered_map>

struct RenderItem;
class GeometryStorage;
class MaterialsDataProvider;
class DynamicVerticesProvider;

enum class RenderLayer : int
{
	Opaque = 0,
	Count
};

struct PrimitiveModel
{
	std::string mesh;
	std::string submesh;
	std::string  material;
	std::vector<float> position;
	std::vector<float> scaling;
	std::vector<float> texture;
};

#pragma once
class ObjectsDataProvider
{
public:
	ObjectsDataProvider(
		std::unique_ptr <GeometryStorage> geometryStorage, 
		MaterialsDataProvider* materialsDataProvider,
		DynamicVerticesProvider* dynamicVerticesProvider
	);
	~ObjectsDataProvider();

	std::vector<RenderItem*> renderItems();
	std::vector<RenderItem*> renderItemsForLayer(RenderLayer layer);
	std::vector<RenderItem*> getRenderItemsWithDynamicVertexBuffer() { return renderItemsWithDynamicVertexBuffer; }

	
	void createPrimitive(
		std::string meshName,
		std::string subMeshName,
		std::string material,
		std::vector<float> position,
		std::vector<float> scaling,
		std::vector<float> textureTransform
	);

	void createPrimitive(PrimitiveModel primitiveModel);

private:
	std::unique_ptr <GeometryStorage> geometryStorage;
	MaterialsDataProvider* materialsDataProvider;
	DynamicVerticesProvider* dynamicVerticesProvider;

	std::vector<std::shared_ptr<RenderItem>> allRitems;
	std::vector<RenderItem*> ritemLayer[(int)RenderLayer::Count]; // Render items divided by PSO.
	std::vector<RenderItem*> renderItemsWithDynamicVertexBuffer;

	int itemIndex = 0;

	void buildRenderItemsForShapes();
	void buildRenderItemsForLandAndWaves();
};


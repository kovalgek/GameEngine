class ObjectsDataProvider;
class GeometryStorage;
class MaterialsDataProvider;
class DynamicVerticesProvider;
class RenderItemTemplatesProvider;

#pragma once
class ObjectsDataProviderConfigurator
{
public:
	ObjectsDataProviderConfigurator(const RenderItemTemplatesProvider& renderItemTemplatesProvider);
	~ObjectsDataProviderConfigurator() = default;

	void configure(ObjectsDataProvider &objectsDataProvider);
private:
	void buildRenderItemsForShapes(ObjectsDataProvider& objectsDataProvider);
	void buildRenderItemsForLandAndWaves(ObjectsDataProvider& objectsDataProvider);
	void buildRenderItemsForStencil(ObjectsDataProvider& objectsDataProvider);

	const RenderItemTemplatesProvider& renderItemTemplatesProvider;
};


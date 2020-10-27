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
	//	const GeometryStorage& geometryStorage,
	//	const MaterialsDataProvider& materialsDataProvider,
	//	const DynamicVerticesProvider& dynamicVerticesProvider
	//);

	~ObjectsDataProviderConfigurator() = default;

	void configure(ObjectsDataProvider &objectsDataProvider);
private:
	void buildRenderItemsForShapes(ObjectsDataProvider& objectsDataProvider);
	//void buildRenderItemsForLandAndWaves();
	//void buildStencilDemoRenderItems();

	//const GeometryStorage& geometryStorage;
	//const MaterialsDataProvider& materialsDataProvider;
	//const DynamicVerticesProvider& dynamicVerticesProvider;

	const RenderItemTemplatesProvider& renderItemTemplatesProvider;
};


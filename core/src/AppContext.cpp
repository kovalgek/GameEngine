#include "AppContext.h"

#include "GPUService.h"
#include "Renderer.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "MaterialsDataProvider.h"
#include "DynamicVerticesProvider.h"
#include "FrameResourceUpdater.h"
#include "ViewController.h"
#include "GeometryStorageConfigurator.h"
#include "MaterialsDataProviderConfigurator.h"
#include "RenderItemTemplatesProvider.h"
#include "OBJFileLoader.h"

AppContext::AppContext(
	std::unique_ptr<GPUService> gpuService,
	std::unique_ptr<Renderer> renderer,
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider,
	std::unique_ptr<ObjectsDataProvider> objectsDataProvider,
	std::unique_ptr<MaterialsDataProvider> materialsDataProvider,
	std::unique_ptr<DynamicVerticesProvider> dynamicVerticesProvider,
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater,
	std::unique_ptr<ViewController> viewController,
	std::unique_ptr<GeometryStorageConfigurator> geometryStorageConfigurator,
	std::unique_ptr<MaterialsDataProviderConfigurator> materialsDataProviderConfigurator,
	std::unique_ptr<RenderItemTemplatesProvider> renderItemTemplatesProvider,
	std::unique_ptr<OBJFileLoader> objFileLoader
) :
	gpuService{ std::move(gpuService) },
	renderer{ std::move(renderer) },
	mainPassDataProvider{ std::move(mainPassDataProvider) },
	objectsDataProvider{ std::move(objectsDataProvider) },
	materialsDataProvider{ std::move(materialsDataProvider) },
	frameResourceUpdater{ std::move(frameResourceUpdater) },
	dynamicVerticesProvider{ std::move(dynamicVerticesProvider) },
	viewController{ std::move(viewController) },
	geometryStorageConfigurator{ std::move(geometryStorageConfigurator) },
	materialsDataProviderConfigurator{ std::move(materialsDataProviderConfigurator) },
	renderItemTemplatesProvider{ std::move(renderItemTemplatesProvider) },
	objFileLoader { std::move(objFileLoader) }
{

}

AppContext::~AppContext() = default;
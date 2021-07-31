#include "AppContext.h"

#include "GPUService.h"
#include "Renderer.h"
#include "MainPassDataProvider.h"
#include "MaterialsDataProvider.h"
#include "DynamicVerticesProvider.h"
#include "FrameResourceUpdater.h"
#include "ViewController.h"
#include "GeometryStorageConfigurator.h"
#include "GeometryGenerator.h"
#include "MaterialsDataProviderConfigurator.h"
#include "OBJFileLoader.h"
#include "Scene.h"
#include "RingBuffer.h"
#include "FrameResource.h"

AppContext::AppContext(
	std::unique_ptr<GPUService> gpuService,
	std::unique_ptr<Renderer> renderer,
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider,
	std::unique_ptr<MaterialsDataProvider> materialsDataProvider,
	std::unique_ptr<DynamicVerticesProvider> dynamicVerticesProvider,
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater,
	std::unique_ptr<RingBuffer<FrameResource>> ringBuffer,
	std::unique_ptr<ViewController> viewController,
	std::unique_ptr<GeometryStorageConfigurator> geometryStorageConfigurator,
	std::unique_ptr<GeometryStorage> geometryStorage,
	std::unique_ptr<GeometryGenerator> geometryGenerator,
	std::unique_ptr<MaterialsDataProviderConfigurator> materialsDataProviderConfigurator,
	std::unique_ptr<OBJFileLoader> objFileLoader,
	std::unique_ptr<Scene> scene
) :
	gpuService{ std::move(gpuService) },
	renderer{ std::move(renderer) },
	mainPassDataProvider{ std::move(mainPassDataProvider) },
	materialsDataProvider{ std::move(materialsDataProvider) },
	frameResourceUpdater{ std::move(frameResourceUpdater) },
	ringBuffer{ std::move(ringBuffer) },	
	dynamicVerticesProvider{ std::move(dynamicVerticesProvider) },
	viewController{ std::move(viewController) },
	geometryStorageConfigurator{ std::move(geometryStorageConfigurator) },
	geometryStorage { std::move(geometryStorage) },
	geometryGenerator { std::move(geometryGenerator) },
	materialsDataProviderConfigurator{ std::move(materialsDataProviderConfigurator) },
	objFileLoader { std::move(objFileLoader) },
	scene{ std::move(scene) }
{
}

AppContext::~AppContext() = default;
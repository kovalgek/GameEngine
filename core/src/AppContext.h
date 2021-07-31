#pragma once

#include <memory>

class GPUService;
class Renderer;
class MainPassDataProvider;
class MaterialsDataProvider;
class DynamicVerticesProvider;
class FrameResourceUpdater;
class ViewController;
class GeometryStorageConfigurator;
class GeometryStorage;
class GeometryGenerator;
class MaterialsDataProviderConfigurator;
class OBJFileLoader;
class Scene;
template<typename Element> class RingBuffer;
struct FrameResource;

class AppContext
{
public:
	AppContext(
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
	);
	~AppContext();

	GPUService* getGPUService() { return gpuService.get(); }
	Renderer* getRenderer() { return renderer.get(); }
	MainPassDataProvider* getMainPassDataProvider() { return mainPassDataProvider.get(); }
	MaterialsDataProvider* getMaterialsDataProvider() { return materialsDataProvider.get(); }
	DynamicVerticesProvider* getDynamicVerticesProvider() { return dynamicVerticesProvider.get(); }
	FrameResourceUpdater* getFrameResourceUpdater() { return frameResourceUpdater.get(); }
	RingBuffer<FrameResource> &getRingBuffer() { return *ringBuffer.get(); }
	ViewController* getViewController() { return viewController.get(); }
	GeometryStorageConfigurator* getGeometryStorageConfigurator() { return geometryStorageConfigurator.get(); }
	MaterialsDataProviderConfigurator* getMaterialsDataProviderConfigurator() { return materialsDataProviderConfigurator.get(); }
	OBJFileLoader* getOBJFileLoader() { return objFileLoader.get(); }

private:
	std::unique_ptr<GPUService> gpuService;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider;
	std::unique_ptr<MaterialsDataProvider> materialsDataProvider;
	std::unique_ptr<DynamicVerticesProvider> dynamicVerticesProvider;
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater;
	std::unique_ptr<RingBuffer<FrameResource>> ringBuffer;
	std::unique_ptr<ViewController> viewController;
	std::unique_ptr<GeometryStorageConfigurator> geometryStorageConfigurator;
	std::unique_ptr<GeometryStorage> geometryStorage;
	std::unique_ptr<GeometryGenerator> geometryGenerator;
	std::unique_ptr<MaterialsDataProviderConfigurator> materialsDataProviderConfigurator;
	std::unique_ptr<OBJFileLoader> objFileLoader;
	std::unique_ptr<Scene> scene;
};
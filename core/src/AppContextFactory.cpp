#include "AppContextFactory.h"

#include "MainPassDataProvider.h"
#include "FrameResourceUpdater.h"

#include "MaterialsDataProvider.h"
#include "MaterialsDataProviderConfigurator.h"
#include "DynamicVerticesProvider.h"
#include "GeometryStorage.h"
#include "GeometryStorageConfigurator.h"
#include "AppFacade.h"
#include "SrvHeapProvider.h"
#include "TexturesProvider.h"
#include "d3dUtil.h"
#include "ViewController.h"
#include "AppContext.h"
#include "RendererFactory.h"


#include "OBJFileLoader.h"
#include <dxgi1_4.h>

#include "GPUServiceFactory.h"
#include "GPUService.h"
#include "GeometryGenerator.h"
#include "Renderer.h"

#include "Scene.h"
#include "RingBuffer.h"
#include "FrameResource.h"
#include "ConstantBufferUpdating.h"
#include "MainPassConstantBufferUpdater.h"
#include "ObjectConstantBufferUpdater.h"
#include "MaterialConstantBufferUpdater.h"

using Microsoft::WRL::ComPtr;

std::unique_ptr<AppContext> AppContextFactory::appContext(HWND mainWindowHandle)
{
	ComPtr<IDXGIFactory4> dxgiFactory;
	AppContextFactory::createDXGIFactory(dxgiFactory.GetAddressOf());

	auto gpuService = GPUServiceFactory::getGPUService(dxgiFactory.Get());

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(gpuService->getCommandList()->Reset(gpuService->getCommandAllocator(), nullptr));

	std::unique_ptr<AppContext> appContext = halfBakedAppContext(mainWindowHandle, std::move(gpuService));

	// Execute the initialization commands.
	ThrowIfFailed(appContext->getGPUService()->getCommandList()->Close());
	ID3D12CommandList* cmdsLists[] = { appContext->getGPUService()->getCommandList() };
	appContext->getGPUService()->getCommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	appContext->getGPUService()->flushCommandQueue();

	return appContext;
}

void AppContextFactory::createDXGIFactory(IDXGIFactory4** dxgiFactory)
{
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory)));
}

std::unique_ptr<AppContext> AppContextFactory::halfBakedAppContext(HWND mainWindowHandle, std::unique_ptr<GPUService> gpuService)
{


	auto geometryGenerator = std::make_unique<GeometryGenerator>();

	auto geometryStorageConfigurator = std::make_unique<GeometryStorageConfigurator>(
		*geometryGenerator
	);

	auto geometryStorage = std::make_unique<GeometryStorage>(
		gpuService->getDevice(),
		gpuService->getCommandList()
	);

	geometryStorageConfigurator->configure(*geometryStorage);
	
	auto materialsDataProvider = std::make_unique<MaterialsDataProvider>();

	auto materialsDataProviderConfigurator = std::make_unique<MaterialsDataProviderConfigurator>();
	materialsDataProviderConfigurator->configure(*materialsDataProvider);

	auto objFileLoader = std::make_unique<OBJFileLoader>(
		*geometryStorage,
		*materialsDataProvider
	);

	auto dynamicVerticesProvider = std::make_unique<DynamicVerticesProvider>();
	geometryStorageConfigurator->configure(*geometryStorage, *dynamicVerticesProvider);


	auto scene = std::make_unique<Scene>(*geometryStorage, *materialsDataProvider);


	auto materials = materialsDataProvider->getMaterials();


	auto renderView = scene->renderView();
	
	

	auto mainPassDataProvider = std::make_unique<MainPassDataProvider>();

	auto texturesProvider = std::make_unique<TexturesProvider>(
		gpuService->getDevice(),
		gpuService->getCommandList()
	);
	auto srvHeapProvider = std::make_unique<SrvHeapProvider>(
		gpuService->getDevice(),
		std::move(texturesProvider)
	);

	auto viewController = std::make_unique<ViewController>(
		mainWindowHandle,
		gpuService->getDevice(),
		gpuService->getCommandList(),
		*srvHeapProvider,
		*mainPassDataProvider,
		*materialsDataProvider,
		*geometryStorage,
		*scene
	);


	auto renderer = RendererFactory::getRenderer(
		mainWindowHandle,
		*gpuService,
		std::move(srvHeapProvider),
		*viewController,
		*scene
	);

	size_t frameCount = 3;

	auto ringBuffer = std::make_unique<RingBuffer<FrameResource>>(frameCount);

	for (int i = 0; i < frameCount; ++i) {
		auto frameResource = std::make_unique<FrameResource>(
			gpuService->getDevice(),
			2,
			renderView.size(),
			(UINT)materials.size());

		ringBuffer->offer(std::move(frameResource));
	}

	std::vector<std::unique_ptr<ConstantBufferUpdating>> constantBufferUpdaters;

	auto mainPassConstantBufferUpdater = std::make_unique<MainPassConstantBufferUpdater>(*mainPassDataProvider);
	constantBufferUpdaters.push_back(std::move(mainPassConstantBufferUpdater));

	auto objectConstantBufferUpdater = std::make_unique<ObjectConstantBufferUpdater>(*scene);
	constantBufferUpdaters.push_back(std::move(objectConstantBufferUpdater));

	auto materialConstantBufferUpdater = std::make_unique<MaterialConstantBufferUpdater>(*materialsDataProvider);
	constantBufferUpdaters.push_back(std::move(materialConstantBufferUpdater));

	auto frameResourceUpdater = std::make_unique<FrameResourceUpdater>(
		*gpuService,		
		std::move(constantBufferUpdaters)
	);

	return std::make_unique<AppContext>(
		std::move(gpuService),
		std::move(renderer),
		std::move(mainPassDataProvider),
		std::move(materialsDataProvider),
		std::move(dynamicVerticesProvider),
		std::move(frameResourceUpdater),
		std::move(ringBuffer),
		std::move(viewController),
		std::move(geometryStorageConfigurator),
		std::move(geometryStorage),
		std::move(geometryGenerator),
		std::move(materialsDataProviderConfigurator),
		std::move(objFileLoader),
		std::move(scene)
	);
}
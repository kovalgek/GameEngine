#include "AppContextFactory.h"

#include "Application.h"
#include "Renderer.h"
#include "PipleneStateData.h"
#include "FrameResourceController.h"
#include "MainPassDataProvider.h"
#include "FrameResourceUpdater.h"
#include "ObjectsDataProvider.h"
#include "MaterialsDataProvider.h"
#include "DynamicVerticesProvider.h"
#include "GeometryStorage.h"
#include "GeometryStorageFactory.h"
#include "AppFacade.h"
#include "Waves.h"
#include "SrvHeapProvider.h"
#include "TexturesProvider.h"
#include "d3dUtil.h"
#include "ViewController.h"
#include "AppContext.h"
#include "RendererFactory.h"
#include "GPUServiceFactory.h"
#include "GPUService.h"

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

	auto geometryStorageFactory = std::make_unique<GeometryStorageFactory>(
		gpuService->getDevice(),
		gpuService->getCommandList(),
		std::move(geometryGenerator)
	);

	auto geometryStorage = geometryStorageFactory->getGeometryStorage();

	auto waves = geometryStorage->getWaves();

	auto materialsDataProvider = std::make_unique<MaterialsDataProvider>();
	auto dynamicVerticesProvider = std::make_unique<DynamicVerticesProvider>();

	auto objectsDataProvider = std::make_unique<ObjectsDataProvider>(
		std::move(geometryStorage),
		*materialsDataProvider,
		*dynamicVerticesProvider
	);

	auto materials = materialsDataProvider->getMaterials();
	auto renderItems = objectsDataProvider->renderItems();

	auto frameResourceController = std::make_unique<FrameResourceController>(
		gpuService->getDevice(),
		1,
		(UINT)renderItems.size(),
		(UINT)materials.size(),
		waves->VertexCount()
	);

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
		*objectsDataProvider,
		*materialsDataProvider,
		*objectsDataProvider->getGeometryStorage()
		);

	auto renderer = RendererFactory::getRenderer(
		mainWindowHandle,
		*gpuService,
		*frameResourceController,
		*objectsDataProvider,
		std::move(srvHeapProvider),
		*viewController);

	auto frameResourceUpdater = std::make_unique<FrameResourceUpdater>(
		std::move(frameResourceController),
		*gpuService,
		*mainPassDataProvider,
		*objectsDataProvider,
		*materialsDataProvider,
		waves);

	return std::make_unique<AppContext>(
		std::move(gpuService),
		std::move(renderer),
		std::move(mainPassDataProvider),
		std::move(objectsDataProvider),
		std::move(materialsDataProvider),
		std::move(dynamicVerticesProvider),
		std::move(frameResourceUpdater),
		std::move(viewController));
}
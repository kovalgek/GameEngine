#include "AppFacadeFactory.h"
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
#include "AppFacade.h"
#include "Waves.h"
#include "SrvHeapProvider.h"
#include "TexturesProvider.h"
#include "d3dUtil.h"
#include "ImGuiController.h"

std::unique_ptr<AppFacade> AppFacadeFactory::appFacade(HWND mainWindowHandle)
{
	// The order is importaint
	auto application = std::make_unique<Application>(mainWindowHandle);


	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(application->getCommandList()->Reset(application->getCommandAllocator(), nullptr));


	auto pipleneStateData = std::make_unique <PipleneStateData>(
		application->getDevice(),
		application->getBackBufferFormat(), 
		application->getDepthStencilFormat(), 
		application->getMsaa4xState(),
		application->getMsaa4xQuality()
		);

	auto geometryStorage = std::make_unique <GeometryStorage>(
		application->getDevice(),
		application->getCommandList()
		);

	auto waves = geometryStorage->getWaves();

	auto materialsDataProvider = std::make_unique<MaterialsDataProvider>();
	auto dynamicVerticesProvider = std::make_unique<DynamicVerticesProvider>();

	auto objectsDataProvider = std::make_unique<ObjectsDataProvider>(std::move(geometryStorage), materialsDataProvider.get(), dynamicVerticesProvider.get());

	auto materials = materialsDataProvider->getMaterials();
	auto renderItems = objectsDataProvider->renderItems();

	auto frameResourceController = std::make_unique<FrameResourceController>(application->getDevice(), 1, (UINT)renderItems.size(), (UINT)materials.size(), waves->VertexCount());

	auto mainPassDataProvider = std::make_unique<MainPassDataProvider>();


	auto texturesProvider = std::make_unique<TexturesProvider>(application->getDevice(), application->getCommandList());
	auto srvHeapProvider = std::make_unique<SrvHeapProvider>(application->getDevice(), std::move(texturesProvider));


	auto imGuiController = std::make_unique<ImGuiController>(application.get(), srvHeapProvider.get(), mainWindowHandle, mainPassDataProvider.get(), objectsDataProvider.get());

	// Execute the initialization commands.
	ThrowIfFailed(application->getCommandList()->Close());
	ID3D12CommandList* cmdsLists[] = { application->getCommandList() };
	application->getCommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	application->flushCommandQueue();


	auto renderer = std::make_unique <Renderer>(
		application.get(),
		std::move(pipleneStateData),
		frameResourceController.get(),
		objectsDataProvider.get(),
		std::move(srvHeapProvider),
		std::move(imGuiController));

	auto frameResourceUpdater = std::make_unique<FrameResourceUpdater>(
		std::move(frameResourceController),
		application->getFence(),
		mainPassDataProvider.get(),
		objectsDataProvider.get(),
		materialsDataProvider.get(),
		waves);

	auto appFacade = std::make_unique<AppFacade>(
		std::move(application),
		std::move(renderer),
		std::move(mainPassDataProvider),
		std::move(objectsDataProvider),
		std::move(materialsDataProvider),
		std::move(frameResourceUpdater),
		std::move(dynamicVerticesProvider));

	return std::move(appFacade);
}
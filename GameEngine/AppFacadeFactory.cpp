#include "AppFacadeFactory.h"
#include "Application.h"
#include "Renderer.h"
#include "PipleneStateData.h"
#include "FrameResourceController.h"
#include "MainPassDataProvider.h"
#include "FrameResourceUpdater.h"
#include "ObjectsDataProvider.h"
#include "MaterialsDataProvider.h"
#include "GeometryStorage.h"
#include "AppFacade.h"
#include "Waves.h"
#include "TexturesController.h"
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
		application.get(),
		application->getDevice(),
		application->getCommandList(),
		application->getCommandAllocator(),
		application->getCommandQueue()
		);

	auto waves = geometryStorage->getWaves();

	auto materialsDataProvider = std::make_unique<MaterialsDataProvider>();
	auto objectsDataProvider = std::make_unique<ObjectsDataProvider>(std::move(geometryStorage), materialsDataProvider.get());

	auto materials = materialsDataProvider->getMaterials();
	auto renderItems = objectsDataProvider->renderItems();

	auto frameResourceController = std::make_unique<FrameResourceController>(application->getDevice(), 1, (UINT)renderItems.size(), (UINT)materials.size(), waves->VertexCount());

	auto mainPassDataProvider = std::make_unique<MainPassDataProvider>();

	auto texturesController = std::make_unique<TexturesController>(application->getDevice(), application->getCommandList());


	auto imGuiController = std::make_unique<ImGuiController>(application.get(), texturesController.get(), mainWindowHandle);

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
		std::move(texturesController),
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
		std::move(frameResourceUpdater));

	return std::move(appFacade);
}
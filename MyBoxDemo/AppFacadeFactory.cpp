#include "AppFacadeFactory.h"
#include "Application.h"
#include "MainScene.h"

#include "PipleneStateData.h"
#include "FrameResourceController.h"
#include "MainPassDataProvider.h"
#include "FrameResourceUpdater.h"
#include "ObjectsDataProvider.h"


std::unique_ptr<AppFacade> AppFacadeFactory::appFacade(HWND mainWindowHandle)
{
	auto application = std::make_shared<Application>(mainWindowHandle);

	auto pipleneStateData = std::make_unique <PipleneStateData>(
		application->getDevice(),
		application->getBackBufferFormat(), 
		application->getDepthStencilFormat(), 
		application->getMsaa4xState(),
		application->getMsaa4xQuality()
		);

	auto geometryStorage = std::make_unique <GeometryStorage>(
		application->getDevice(),
		application->getCommandList(),
		application->getCommandAllocator(),
		application->getCommandQueue()
		);

	auto objectsDataProvider = std::make_shared<ObjectsDataProvider>(std::move(geometryStorage));

	auto renderItems = objectsDataProvider->renderItems();
	auto frameResourceController = std::make_shared<FrameResourceController>(application->getDevice(), 1, (UINT)renderItems.size());

	auto mainPassDataProvider = std::make_shared<MainPassDataProvider>();

	auto frameResourceUpdater = std::make_unique<FrameResourceUpdater>(
		frameResourceController,
		application->getFence(),
		mainPassDataProvider,
		objectsDataProvider);

	auto mainScene = std::make_unique <MainScene>(
		application,
		std::move(pipleneStateData),
		frameResourceController,
		objectsDataProvider);

	auto appFacade = std::make_unique<AppFacade>(
		application,
		std::move(mainScene),
		mainPassDataProvider,
		objectsDataProvider,
		std::move(frameResourceUpdater));

	return std::move(appFacade);
}
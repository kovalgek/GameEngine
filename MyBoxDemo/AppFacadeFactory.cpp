#include "AppFacadeFactory.h"
#include "Application.h"
#include "MainScene.h"
#include "PipleneStateData.h"
#include "FrameResourceController.h"
#include "MainPassDataProvider.h"
#include "FrameResourceUpdater.h"
#include "ObjectsDataProvider.h"
#include "GeometryStorage.h"
#include "AppFacade.h"

std::unique_ptr<AppFacade> AppFacadeFactory::appFacade(HWND mainWindowHandle)
{
	// The order is importaint

	auto application = std::make_unique<Application>(mainWindowHandle);

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

	auto objectsDataProvider = std::make_unique<ObjectsDataProvider>(std::move(geometryStorage));

	auto renderItems = objectsDataProvider->renderItems();
	auto frameResourceController = std::make_unique<FrameResourceController>(application->getDevice(), 1, (UINT)renderItems.size());

	auto mainPassDataProvider = std::make_unique<MainPassDataProvider>();

	auto mainScene = std::make_unique <MainScene>(
		application.get(),
		std::move(pipleneStateData),
		frameResourceController.get(),
		objectsDataProvider.get());

	auto frameResourceUpdater = std::make_unique<FrameResourceUpdater>(
		std::move(frameResourceController),
		application->getFence(),
		mainPassDataProvider.get(),
		objectsDataProvider.get());

	auto appFacade = std::make_unique<AppFacade>(
		std::move(application),
		std::move(mainScene),
		std::move(mainPassDataProvider),
		std::move(objectsDataProvider),
		std::move(frameResourceUpdater));

	return std::move(appFacade);
}
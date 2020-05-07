#include "AppFacadeFactory.h"
#include "Application.h"
#include "MainScene.h"

#include "PipleneStateData.h"
#include "FrameResourceController.h"

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

	auto renderItems = geometryStorage->renderItems();

	auto frameResourceController = std::make_unique<FrameResourceController>(application->getDevice(), 1, (UINT)renderItems.size());

	auto mainScene = std::make_unique <MainScene>(
		application,
		std::move(pipleneStateData),
		std::move(geometryStorage),
		std::move(frameResourceController),
		std::move(renderItems));

	auto appFacade = std::make_unique<AppFacade>(application, std::move(mainScene));
	return std::move(appFacade);
}
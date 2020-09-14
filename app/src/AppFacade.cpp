#include "AppFacade.h"
#include "Application.h"
#include "Renderer.h"
#include "GameTimer.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "FrameResourceUpdater.h"
#include "MaterialsDataProvider.h"
#include "DynamicVerticesProvider.h"
#include "ViewController.h"

AppFacade::AppFacade(
	std::unique_ptr<Application> appContext,
	std::unique_ptr<Renderer> renderer,
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider,
	std::unique_ptr<ObjectsDataProvider> objectsDataProvider,
	std::unique_ptr<MaterialsDataProvider> materialsDataProvider,
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater,
	std::unique_ptr<DynamicVerticesProvider> dynamicVerticesProvider,
	std::unique_ptr<ViewController> viewController
) :
	appContext { std::move(appContext) },
	renderer{ std::move(renderer) },
	mainPassDataProvider { std::move(mainPassDataProvider) },
	objectsDataProvider { std::move(objectsDataProvider) },
	materialsDataProvider { std::move(materialsDataProvider) },
	frameResourceUpdater { std::move(frameResourceUpdater) },
	dynamicVerticesProvider { std::move(dynamicVerticesProvider) },
	viewController{ std::move(viewController) }
{

}

AppFacade::~AppFacade() = default;

void AppFacade::update(const GameTimer& gameTimer)
{
	frameResourceUpdater->update(gameTimer);
	dynamicVerticesProvider->update(gameTimer);
	viewController->update();
	renderer->draw(gameTimer);
}

void AppFacade::onResize(int clientWidth, int clientHeight)
{
	appContext->onResize(clientWidth, clientHeight);
	viewController->onWindowResize(clientWidth, clientHeight);
}

void AppFacade::onMouseDown(int x, int y)
{
	viewController->onMouseDown(x, y);
}

void AppFacade::onMouseUp(int x, int y)
{
}

void AppFacade::onMouseMove(int btnState, int x, int y)
{
	viewController->onMouseMove(btnState, x, y);
}
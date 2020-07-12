#include "AppFacade.h"
#include "Application.h"
#include "Renderer.h"
#include "GameTimer.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "FrameResourceUpdater.h"
#include "MaterialsDataProvider.h"

AppFacade::AppFacade(
	std::unique_ptr<Application> appContext,
	std::unique_ptr<Renderer> renderer,
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider,
	std::unique_ptr<ObjectsDataProvider> objectsDataProvider,
	std::unique_ptr<MaterialsDataProvider> materialsDataProvider,
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater
) :
	appContext { std::move(appContext) },
	renderer{ std::move(renderer) },
	mainPassDataProvider { std::move(mainPassDataProvider) },
	objectsDataProvider { std::move(objectsDataProvider) },
	materialsDataProvider { std::move(materialsDataProvider) },
	frameResourceUpdater { std::move(frameResourceUpdater) }
{

}

AppFacade::~AppFacade()
{

}

void AppFacade::update(const GameTimer& gameTimer)
{
	mainPassDataProvider->updateCamera();
	frameResourceUpdater->update(gameTimer);
	renderer->draw(gameTimer);
}

void AppFacade::onResize(int clientWidth, int clientHeight)
{
	appContext->onResize(clientWidth, clientHeight);
	mainPassDataProvider->onResize(clientWidth, clientHeight);
}

void AppFacade::onMouseDown(int x, int y)
{
	mainPassDataProvider->onMouseDown(x, y);
	objectsDataProvider->onMouseDown(x, y);
}

void AppFacade::onMouseUp(int x, int y)
{
}

void AppFacade::onMouseMove(int btnState, int x, int y)
{
	mainPassDataProvider->onMouseMove(btnState, x, y);
}
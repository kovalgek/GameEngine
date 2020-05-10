#include "AppFacade.h"
#include "Application.h"
#include "MainScene.h"
#include "GameTimer.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "FrameResourceUpdater.h"

AppFacade::AppFacade(
	std::shared_ptr<Application> appContext,
	std::unique_ptr<MainScene> mainScene,
	std::shared_ptr<MainPassDataProvider> mainPassDataProvider,
	std::shared_ptr<ObjectsDataProvider> objectsDataProvider,
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater) :
	appContext { appContext },
	mainScene { std::move(mainScene) },
	mainPassDataProvider { mainPassDataProvider },
	objectsDataProvider { objectsDataProvider },
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

	mainScene->draw(gameTimer);
}

void AppFacade::onResize(int clientWidth, int clientHeight)
{
	appContext->onResize(clientWidth, clientHeight);
	mainPassDataProvider->onResize(clientWidth, clientHeight);
}

void AppFacade::onMouseDown(int x, int y)
{
	mainPassDataProvider->onMouseDown(x, y);
}

void AppFacade::onMouseUp(int x, int y)
{
}

void AppFacade::onMouseMove(int x, int y)
{
	mainPassDataProvider->onMouseMove(x, y);
}
#include "AppFacade.h"
#include "ApplicationContext.h"
#include "MainScene.h"
#include "GameTimer.h"

AppFacade::AppFacade(ApplicationContext* appContext, MainScene* mainScene)
{
	this->appContext = appContext;
	this->mainScene = mainScene;
}

void AppFacade::update(const GameTimer& gameTimer)
{
	mainScene->update(gameTimer);
	mainScene->draw(gameTimer);
}

void AppFacade::onResize(int clientWidth, int clientHeight)
{
	appContext->onResize(clientWidth, clientHeight);
	mainScene->onResize(clientWidth, clientHeight);
}
#include "AppFacade.h"
#include "Application.h"
#include "MainScene.h"
#include "GameTimer.h"

AppFacade::AppFacade(Application* appContext, MainScene* mainScene)
{
	this->appContext = appContext;
	this->mainScene = mainScene;
}

void AppFacade::update(const GameTimer& gameTimer)
{
	mainScene->update(gameTimer);
	mainScene->draw(gameTimer);
	appContext->draw();
}

void AppFacade::onResize(int clientWidth, int clientHeight)
{
	appContext->onResize(clientWidth, clientHeight);
	mainScene->onResize(clientWidth, clientHeight);
}

void AppFacade::onMouseDown(int x, int y)
{
	mainScene->onMouseDown(x, y);
}

void AppFacade::onMouseUp(int x, int y)
{
	mainScene->onMouseUp(x, y);
}

void AppFacade::onMouseMove(int x, int y)
{
	mainScene->onMouseMove(x, y);
}
#include "AppFacade.h"
#include "AppContextFactory.h"
#include "AppContext.h"
#include "Renderer.h"
#include "GameTimer.h"
#include "FrameResourceUpdater.h"
#include "DynamicVerticesProvider.h"
#include "ViewController.h"
#include "RingBuffer.h"

AppFacade::AppFacade(HWND mainWindowHandle)
{
	appContext = AppContextFactory::appContext(mainWindowHandle);
}

AppFacade::~AppFacade() = default;

void AppFacade::update(const GameTimer& gameTimer)
{
	auto& ringBuffer = appContext->getRingBuffer();
	auto& frameResource = ringBuffer.poll();

	appContext->getFrameResourceUpdater()->update(frameResource, gameTimer);
	appContext->getDynamicVerticesProvider()->update(gameTimer);
	appContext->getViewController()->update();

	appContext->getRenderer()->draw(frameResource, gameTimer);
}

void AppFacade::onResize(int clientWidth, int clientHeight)
{
	appContext->getRenderer()->onResize(clientWidth, clientHeight);
	appContext->getViewController()->onWindowResize(clientWidth, clientHeight);
}

void AppFacade::onMouseDown(int x, int y)
{
	appContext->getViewController()->onMouseDown(x, y);
}

void AppFacade::onMouseUp(int x, int y)
{
}

void AppFacade::onMouseMove(int btnState, int x, int y)
{
	appContext->getViewController()->onMouseMove(btnState, x, y);
}
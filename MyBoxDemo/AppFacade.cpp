#include "AppFacade.h"
#include "Application.h"
#include "MainScene.h"
#include "GameTimer.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "FrameResourceUpdater.h"

AppFacade::AppFacade(
	std::unique_ptr<Application> appContext,
	std::unique_ptr<MainScene> mainScene,
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider,
	std::unique_ptr<ObjectsDataProvider> objectsDataProvider,
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater
) :
	appContext { std::move(appContext) },
	mainScene { std::move(mainScene) },
	mainPassDataProvider { std::move(mainPassDataProvider) },
	objectsDataProvider { std::move(objectsDataProvider) },
	frameResourceUpdater { std::move(frameResourceUpdater) }
{

}

AppFacade::~AppFacade()
{

}

void AppFacade::update(const GameTimer& gameTimer)
{
	mainPassDataProvider->updateCamera();


	//__int64 countsPerSec;
//QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
//double mSecondsPerCount = 1.0 / (double)countsPerSec;

	__int64 time1;
	QueryPerformanceCounter((LARGE_INTEGER*)&time1);



	frameResourceUpdater->update(gameTimer);

	__int64 time2;
	QueryPerformanceCounter((LARGE_INTEGER*)&time2);


	mainScene->draw(gameTimer);


	__int64 time3;
	QueryPerformanceCounter((LARGE_INTEGER*)&time3);


	static float timeElapsed = 0.0f;

	static std::vector<__int64> delta1Vec;
	static std::vector<__int64> delta2Vec;


	__int64 delta1 = time2 - time1;
	__int64 delta2 = time3 - time2;

	delta1Vec.push_back(delta1);
	delta2Vec.push_back(delta2);


	if ((gameTimer.TotalTime() - timeElapsed) >= 1.0f)
	{


		__int64 sum1 = 0;
		for (auto x : delta1Vec) {
			sum1 += x;
		}
		__int64 sum2 = 0;
		for (auto x : delta2Vec) {
			sum2 += x;
		}
		__int64 middleDelta1 = sum1 / delta1Vec.size();
		__int64 middleDelta2 = sum2 / delta2Vec.size();



		std::wstring text = L"delta1=" + std::to_wstring(delta1) + L" midDelta1=" + std::to_wstring(middleDelta1) + L" delta2=" + std::to_wstring(delta2) + L" midDelta2=" + std::to_wstring(middleDelta2) + L"\n";

		::OutputDebugString(text.c_str());


		timeElapsed += 1.0f;
	}
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

void AppFacade::onMouseMove(int x, int y)
{
	mainPassDataProvider->onMouseMove(x, y);
}
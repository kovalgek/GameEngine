class Application;
class MainScene;
class GameTimer;
class MainPassDataProvider;
class FrameResourceUpdater;
class ObjectsDataProvider;
#include <memory>

#pragma once
class AppFacade
{
public:
	AppFacade(
		std::shared_ptr<Application> appContext,
		std::unique_ptr<MainScene> mainScene,
		std::shared_ptr<MainPassDataProvider> mainPassDataProvider,
		std::shared_ptr<ObjectsDataProvider> objectsDataProvider,
		std::unique_ptr<FrameResourceUpdater> frameResourceUpdater);
	~AppFacade();
	void update(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int x, int y);
private:
	std::shared_ptr<Application> appContext;
	std::unique_ptr<MainScene> mainScene;
	std::shared_ptr<MainPassDataProvider> mainPassDataProvider;
	std::shared_ptr<ObjectsDataProvider> objectsDataProvider;
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater;
};


#include <memory>

class Application;
class MainScene;
class GameTimer;
class MainPassDataProvider;
class FrameResourceUpdater;
class ObjectsDataProvider;

#pragma once
class AppFacade
{
public:
	AppFacade(
		std::unique_ptr<Application> appContext,
		std::unique_ptr<MainScene> mainScene,
		std::unique_ptr<MainPassDataProvider> mainPassDataProvider,
		std::unique_ptr<ObjectsDataProvider> objectsDataProvider,
		std::unique_ptr<FrameResourceUpdater> frameResourceUpdater);
	~AppFacade();
	void update(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int x, int y);
private:
	std::unique_ptr<Application> appContext;
	std::unique_ptr<MainScene> mainScene;
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider;
	std::unique_ptr<ObjectsDataProvider> objectsDataProvider;
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater;
};


#include <memory>

class Application;
class Renderer;
class GameTimer;
class MainPassDataProvider;
class FrameResourceUpdater;
class ObjectsDataProvider;
class MaterialsDataProvider;

#pragma once
class AppFacade
{
public:
	AppFacade(
		std::unique_ptr<Application> appContext,
		std::unique_ptr<Renderer> renderer,
		std::unique_ptr<MainPassDataProvider> mainPassDataProvider,
		std::unique_ptr<ObjectsDataProvider> objectsDataProvider,
		std::unique_ptr<MaterialsDataProvider> materialsDataProvider,
		std::unique_ptr<FrameResourceUpdater> frameResourceUpdater);
	~AppFacade();
	void update(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int btnState, int x, int y);
private:
	std::unique_ptr<Application> appContext;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<MainPassDataProvider> mainPassDataProvider;
	std::unique_ptr<ObjectsDataProvider> objectsDataProvider;
	std::unique_ptr<MaterialsDataProvider> materialsDataProvider;
	std::unique_ptr<FrameResourceUpdater> frameResourceUpdater;
};


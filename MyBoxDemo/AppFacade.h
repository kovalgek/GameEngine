class Application;
class MainScene;
class GameTimer;

#include <memory>

#pragma once
class AppFacade
{
public:
	AppFacade(std::shared_ptr<Application> appContext, std::unique_ptr<MainScene> mainScene);
	~AppFacade();
	void update(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int x, int y);
private:
	std::shared_ptr<Application> appContext;
	std::unique_ptr<MainScene> mainScene;
};


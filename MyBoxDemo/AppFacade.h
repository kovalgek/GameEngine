class ApplicationContext;
class MainScene;
class GameTimer;

#pragma once
class AppFacade
{
public:
	AppFacade(ApplicationContext *appContext, MainScene *mainScene);
	void update(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int x, int y);
private:
	ApplicationContext* appContext;
	MainScene* mainScene;
};


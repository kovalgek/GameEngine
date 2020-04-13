
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
private:
	ApplicationContext* appContext;
	MainScene* mainScene;
};


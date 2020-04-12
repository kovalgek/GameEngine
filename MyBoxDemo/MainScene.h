
class ApplicationContext;
class GameTimer;

#pragma once
class MainScene
{
public:
	MainScene(ApplicationContext *appContext);
	void update(const GameTimer& gameTimer);
	void draw(const GameTimer& gameTimer);
};


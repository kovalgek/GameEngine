#include <memory>
#include <windows.h>

class AppContext;
class GameTimer;

#pragma once
class AppFacade
{
public:
	AppFacade(HWND mainWindowHandle);
	~AppFacade();

	void update(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int btnState, int x, int y);

private:

	std::unique_ptr<AppContext> appContext;
};


#include <memory>
#include <windows.h>

class AppFacade;

#pragma once
class AppFacadeFactory
{
public:
	static std::unique_ptr<AppFacade> appFacade(HWND mainWindowHandle);
};


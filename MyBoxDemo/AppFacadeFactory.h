#include <memory>
#include <windows.h>
#include "AppFacade.h"

#pragma once
class AppFacadeFactory
{
public:
	static std::unique_ptr<AppFacade> appFacade(HWND mainWindowHandle);
};


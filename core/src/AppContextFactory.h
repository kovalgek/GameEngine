#include <memory>
#include <windows.h>

class AppContext;
class Application;

#pragma once
class AppContextFactory
{
public:
	static std::unique_ptr<AppContext> appContext(HWND mainWindowHandle);
private:
	static std::unique_ptr<AppContext> halfBakedAppContext(HWND mainWindowHandle, std::unique_ptr<Application> application);
};


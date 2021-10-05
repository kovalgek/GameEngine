#include <memory>
#include <windows.h>

class AppContext;
class GPUService;
struct IDXGIFactory4;

#pragma once
class AppContextFactory
{
public:
	static std::unique_ptr<AppContext> appContext(HWND mainWindowHandle);
private:
	static std::unique_ptr<AppContext> halfBakedAppContext(HWND mainWindowHandle, std::unique_ptr<GPUService> gpuService);
	static void createDXGIFactory(IDXGIFactory4** dxgiFactory);
};
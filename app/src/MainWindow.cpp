#include "MyBoxDemo.h"
#include <wchar.h>
#include <stdio.h>
#include <string>
#include <WindowsX.h>
#include "GameTimer.h"
#include "AppFacade.h"
#include "imgui.h"

#define MAX_LOADSTRING 100

std::wstring mMainWndCaption = L"Parelhoen Engine";

HINSTANCE applicationInstanceHandle = nullptr;
HWND mainWindowHandle = nullptr;
AppFacade* appFacade = nullptr;
bool appPaused = false;
GameTimer timer;

bool      resizing = false;   // are the resize bars being dragged?
bool      minimized = false;  // is the application minimized?
bool      maximized = false;  // is the application maximized?
bool      fullscreenState = false;// fullscreen enabled
int       clientWidth = 800;
int       clientHeight = 600;

WCHAR windowTitle[MAX_LOADSTRING];
WCHAR windowClass[MAX_LOADSTRING];

ATOM                registerClass(HINSTANCE hInstance);
BOOL                initMainWindow(HINSTANCE, int);
int                 runMainLoop(HINSTANCE hInstance);
LRESULT CALLBACK    handleMessage(HWND, UINT, WPARAM, LPARAM);
void                calculateFrameStats();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    wcsncpy_s(windowTitle, MAX_LOADSTRING, L"Box3d", MAX_LOADSTRING - 1);
    wcsncpy_s(windowClass, MAX_LOADSTRING, L"WindowClass", MAX_LOADSTRING - 1);

    registerClass(hInstance);

    if (!initMainWindow(hInstance, nCmdShow))
    {
        return FALSE;
    }

    //try
//    {
		appFacade = new AppFacade(mainWindowHandle);
		appFacade->onResize(clientWidth, clientHeight);

        return runMainLoop(hInstance);
    // }
    // catch (DxException& e)
    // {
    //     MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    //     return 0;
    // }
}

ATOM registerClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = handleMessage;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYBOXDEMO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYBOXDEMO);
    wcex.lpszClassName  = windowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL initMainWindow(HINSTANCE hInstance, int nCmdShow)
{
    applicationInstanceHandle = hInstance;

    mainWindowHandle = CreateWindowW(windowClass, windowTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!mainWindowHandle)
    {
        return FALSE;
    }

    ShowWindow(mainWindowHandle, nCmdShow);
    UpdateWindow(mainWindowHandle);



    return TRUE;
}

int runMainLoop(HINSTANCE hInstance)
{
	MSG msg = { 0 };

	timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();

			if (!appPaused)
			{
				calculateFrameStats();
				appFacade->update(timer);
			}
			else
			{
			    Sleep(100);
			}
		}
	}

    return (int)msg.wParam;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			appPaused = true;
			timer.Stop();
		}
		else
		{
			appPaused = false;
			timer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		clientWidth = LOWORD(lParam);
		clientHeight = HIWORD(lParam);

		if (appFacade)
		{

			if (wParam == SIZE_MINIMIZED)
			{
				appPaused = true;
				minimized = true;
				maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				appPaused = false;
				minimized = false;
				maximized = true;
				appFacade->onResize(clientWidth, clientHeight);
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (minimized)
				{
					appPaused = false;
					minimized = false;
					appFacade->onResize(clientWidth, clientHeight);
				}

				// Restoring from maximized state?
				else if (maximized)
				{
					appPaused = false;
					maximized = false;
					appFacade->onResize(clientWidth, clientHeight);
				}
				else if (resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					appFacade->onResize(clientWidth, clientHeight);
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		appPaused = true;
		resizing = true;
		timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		appPaused = false;
		resizing = false;
		timer.Start();
		appFacade->onResize(clientWidth, clientHeight);
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
		break;
	case WM_MBUTTONDOWN:
		SetCapture(mainWindowHandle);
		appFacade->onMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MBUTTONUP:
		ReleaseCapture();
		appFacade->onMouseUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
		break;
		
	case WM_MOUSEMOVE:
		if ((wParam & MK_MBUTTON) != 0)
		{
			appFacade->onMouseMove(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		//else if ((wParam & MK_RBUTTON) != 0)
		//{
		//	appFacade->onMouseMove(1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		//}
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}

		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void calculateFrameStats()
{
    static int frameCnt = 0;
    static float timeElapsed = 0.0f;

    frameCnt++;

    // Compute averages over one second period.
    if ((timer.TotalTime() - timeElapsed) >= 1.0f)
    {
        float fps = (float)frameCnt; // fps = frameCnt / 1
        float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = mMainWndCaption +
            L"    fps: " + fpsStr +
            L"   mspf: " + mspfStr;

        SetWindowText(mainWindowHandle, windowText.c_str());

        // Reset for next average.
        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}

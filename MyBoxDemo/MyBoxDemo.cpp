// MyBoxDemo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MyBoxDemo.h"
#include <wchar.h>
#include <stdio.h>
#include "BoxApp.h"

#include "d3dUtil.h"

#define MAX_LOADSTRING 100

HINSTANCE applicationInstanceHandle = nullptr;
HWND mainWindowHandle = nullptr;
BoxApp* boxApp = nullptr;

WCHAR windowTitle[MAX_LOADSTRING];
WCHAR windowClass[MAX_LOADSTRING];

ATOM                registerClass(HINSTANCE hInstance);
BOOL                initMainWindow(HINSTANCE, int);
int                 runMainLoop(HINSTANCE hInstance);
LRESULT CALLBACK    handleMessage(HWND, UINT, WPARAM, LPARAM);

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

    

    try
    {
        BoxApp boxApp(mainWindowHandle);

        if (!boxApp.initialize())
            return 0;

        //MainScene mainScene(context);


        return runMainLoop(hInstance);
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
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
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYBOXDEMO));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            //mainScene->update();
            //mainScene->draw();
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK handleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
        boxApp->windowDidActivate();
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

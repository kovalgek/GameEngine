

#include <windows.h>

#define MAX_LOADSTRING 100


class BoxApp
{
public:
	BoxApp();
	bool initialize();
	void update();

	void windowDidActivate();
private:
	HINSTANCE applicationInstanceHandle  = nullptr;
	HWND mainWindowHandle = nullptr;
	WCHAR windowClass[MAX_LOADSTRING];            // the main window class name

	ATOM MyRegisterClass(HINSTANCE hInstance);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};


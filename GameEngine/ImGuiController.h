#include <windows.h>

class Application;
class TexturesController;

#pragma once
class ImGuiController
{
public:
	ImGuiController(Application* application, TexturesController* texturesController, HWND mainWindowHandle);
	void present();
private:
	Application* application;

};


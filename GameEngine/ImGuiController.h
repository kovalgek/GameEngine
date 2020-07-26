#include <windows.h>
#include <DirectXMath.h>

class Application;
class SrvHeapProvider;
class MainPassDataProvider;
class ObjectsDataProvider;

struct CameraModel
{

};

#pragma once
class ImGuiController
{
public:
	ImGuiController(
		Application* application,
		SrvHeapProvider* texturesController,
		HWND mainWindowHandle,
		MainPassDataProvider *mainPassDataProvider,
		ObjectsDataProvider *objectsDataProvider);
	void present();
private:
	Application* application;
	MainPassDataProvider* mainPassDataProvider;
	ObjectsDataProvider* objectsDataProvider;

	float theta = DirectX::XM_PI;
	float phi = DirectX::XM_PIDIV4;
	float radius = 70;

	float hillsCoords[3] = { 0.0f, 0.0f, 0.0f };
	float boxCoords[3] = { 0.0f, 0.0f, 0.0f };
	float sphereCoords[3] = { 0.0f, 0.0f, 0.0f };

};


#include <windows.h>
#include <DirectXMath.h>
#include <string>
#include <memory>
#include <vector>
#include "MainPassModels.h"

class Application;
class SrvHeapProvider;
class MainPassModelsListener;
class GeometryStorage;
class MaterialsDataProvider;
struct PrimitiveModel;
struct CameraModel;
struct LightModel;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;

class CameraViewModel;
class CameraView;
class LightViewModel;
class LightView;
class FogViewModel;
class FogView;
class PrimitiveViewModel;
class PrimitiveView;
class View;
class Scene;

#pragma once
class ViewController
{
public:
	ViewController(
		HWND                      mainWindowHandle,
		ID3D12Device              *device,
		ID3D12GraphicsCommandList *commandList,
		SrvHeapProvider&          srvHeapProvider,
		MainPassModelsListener&	  mainPassModelsListener,	
		MaterialsDataProvider&    materialsDataProvider,
		GeometryStorage&          geometryStorage,
		Scene& scene);
	~ViewController();

	void present();
	void update();

	void onWindowResize(int clientWidth, int clientHeight);
	void onMouseDown(int x, int y);
	void onMouseMove(int btnState, int x, int y);

private:
	ID3D12Device              *device;
	ID3D12GraphicsCommandList *commandList;
	MainPassModelsListener &mainPassModelsListener;
	MaterialsDataProvider &materialsDataProvider;
	GeometryStorage       &geometryStorage;

	CameraModel cameraModel(CameraViewModel* cameraViewModel);
	LightModel lightModel(LightViewModel* lightViewModel);
	FogModel fogModel(FogViewModel* fogViewModel);

//	PrimitiveModel primitiveModel(PrimitiveViewModel *primitiveViewModel);

	POINT lastMousePosition;


	std::unique_ptr<CameraViewModel> cameraViewModel;
	std::unique_ptr<CameraView> cameraView;

	std::unique_ptr<LightViewModel> lightViewModel;
	std::unique_ptr<LightView> lightView;

	std::unique_ptr<FogViewModel> fogViewModel;
	std::unique_ptr<FogView> fogView;

	std::unique_ptr<PrimitiveViewModel> primitiveViewModel;
	std::unique_ptr<PrimitiveView> primitiveView;
	
	std::vector<View*> views;
};


#include <windows.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "ViewModels.h"
#include "MainPassModels.h"

class Application;
class SrvHeapProvider;
class MainPassModelsListener;
class ObjectsDataProvider;
class GeometryStorage;
class MaterialsDataProvider;
struct PrimitiveModel;
struct CameraModel;
struct LightModel;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;

#pragma once
class ViewController
{
public:
	ViewController(
		HWND                      mainWindowHandle,
		ID3D12Device              *device,
		ID3D12GraphicsCommandList *commandList,
		SrvHeapProvider           *srvHeapProvider,	
		MainPassModelsListener	  *mainPassModelsListener,
		ObjectsDataProvider       *objectsDataProvider,		
		MaterialsDataProvider     *materialsDataProvider,
		GeometryStorage           *geometryStorage);

	void present();
	void update();

	void onWindowResize(int clientWidth, int clientHeight);
	void onMouseDown(int x, int y);
	void onMouseMove(int btnState, int x, int y);

private:
	ID3D12Device              *device;
	ID3D12GraphicsCommandList *commandList;
	MainPassModelsListener* mainPassModelsListener;
	ObjectsDataProvider   *objectsDataProvider;
	MaterialsDataProvider *materialsDataProvider;
	GeometryStorage       *geometryStorage;

	std::unordered_map<std::string, std::vector<std::string>> meshes;
	std::vector<std::string> materials;

	void createCameraView();
	void createLightView();
	void createPrimitiveFactoryView();
	void createMeshCombo();
	void createSubmeshCombo();
	void createMaterialCombo();

	CameraViewModel    cameraViewModel;
	PrimitiveViewModel primitiveViewModel;
	LightsViewModel    lightsViewModel;

	void initLightsViewModel(LightsViewModel& lightsViewModel);
	void initPrimitiveViewModel(PrimitiveViewModel& primitiveViewModel);

	CameraModel cameraModel(CameraViewModel cameraViewModel);
	LightModel lightModel(LightsViewModel lightsViewModel);
	PrimitiveModel primitiveModel(PrimitiveViewModel primitiveViewModel);

	POINT lastMousePosition;
};

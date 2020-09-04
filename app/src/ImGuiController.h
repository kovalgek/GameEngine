#include <windows.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "CameraViewModel.h"

class Application;
class SrvHeapProvider;
class MainPassDataProvider;
class ObjectsDataProvider;
class GeometryStorage;
class MaterialsDataProvider;
struct PrimitiveModel;
struct CameraModel;
struct LightModel;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;

struct PrimitiveViewModel
{
	std::string currentMesh;
	std::string currentSubMesh;
	std::string currentMaterial;
	float position[3];
	float scaling[3];
	float texture[3];
};

struct LightsViewModel
{
	float ambient[4];
	float direction[3];
	float strength[3];
};

#pragma once
class ImGuiController
{
public:
	ImGuiController(
		HWND                      mainWindowHandle,
		ID3D12Device              *device,
		ID3D12GraphicsCommandList *commandList,
		SrvHeapProvider           *srvHeapProvider,	
		MainPassDataProvider      *mainPassDataProvider,
		ObjectsDataProvider       *objectsDataProvider,		
		MaterialsDataProvider     *materialsDataProvider,
		GeometryStorage           *geometryStorage);

	void present();
	void update();

private:
	ID3D12Device              *device;
	ID3D12GraphicsCommandList *commandList;
	MainPassDataProvider  *mainPassDataProvider;
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

	void initPrimitiveViewModel(PrimitiveViewModel& primitiveViewModel);
	CameraModel cameraModel(CameraViewModel cameraViewModel);
	LightModel lightModel(LightsViewModel lightsViewModel);
	PrimitiveModel primitiveModel(PrimitiveViewModel primitiveViewModel);
};


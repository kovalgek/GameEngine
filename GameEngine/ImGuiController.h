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
		Application* application,
		SrvHeapProvider* texturesController,
		HWND mainWindowHandle,
		MainPassDataProvider *mainPassDataProvider,
		ObjectsDataProvider *objectsDataProvider,
		GeometryStorage *geometryStorage,
		MaterialsDataProvider * materialsDataProvider);
	void present();
private:
	Application* application;
	MainPassDataProvider* mainPassDataProvider;
	ObjectsDataProvider* objectsDataProvider;
	GeometryStorage* geometryStorage;

	CameraViewModel cameraViewModel;
	PrimitiveViewModel primitiveViewModel;
	LightsViewModel lightsViewModel;

	std::unordered_map<std::string, std::vector<std::string>> meshes;
	std::vector<std::string> materials;

	void createCameraView();
	void createPrimitiveFactoryView();
	void createLightView();

	void createMeshCombo();
	void createSubmeshCombo();
	void createMaterialCombo();

	PrimitiveModel convertModel(PrimitiveViewModel primitiveViewModel);
};


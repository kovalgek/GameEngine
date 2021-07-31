#include "ViewController.h"
#include "SrvHeapProvider.h"
#include "MainPassDataProvider.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"

#include <string.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include <iostream>
#include <dxgi1_4.h>
#include <d3d12.h>


#include "CameraView.h"
#include "CameraViewModel.h"
#include "LightView.h"
#include "LightViewModel.h"
#include "FogView.h"
#include "FogViewModel.h"
#include "PrimitiveViewModel.h"
#include "PrimitiveView.h"
#include "Scene.h"
#include "View.h"
#include "ViewModel.h"

using namespace DirectX;

ViewController::ViewController(
	HWND                      mainWindowHandle,
	ID3D12Device              *device,
	ID3D12GraphicsCommandList *commandList,
	SrvHeapProvider&          srvHeapProvider,
	MainPassModelsListener    &mainPassModelsListener,
	MaterialsDataProvider&    materialsDataProvider,
	GeometryStorage&          geometryStorage,
	Scene& scene) :

	device { device },
	commandList { commandList },
	mainPassModelsListener{ mainPassModelsListener },
	materialsDataProvider { materialsDataProvider },
	geometryStorage { geometryStorage }
{

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(mainWindowHandle);
	ImGui_ImplDX12_Init(device, 3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		srvHeapProvider.getSrvDescriptorHeap(),
		srvHeapProvider.getSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		srvHeapProvider.getSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	ImGui::StyleColorsDark();


	cameraViewModel = std::make_unique<CameraViewModel>();
	cameraView = std::make_unique<CameraView>(*cameraViewModel);
	views.push_back(cameraView.get());

	lightViewModel = std::make_unique<LightViewModel>(scene);
	lightView = std::make_unique<LightView>(*lightViewModel);
	views.push_back(lightView.get());

	fogViewModel = std::make_unique<FogViewModel>();
	fogView = std::make_unique<FogView>(*fogViewModel);
	views.push_back(fogView.get());

	primitiveViewModel = std::make_unique<PrimitiveViewModel>(materialsDataProvider, geometryStorage);
	primitiveView = std::make_unique<PrimitiveView>(*primitiveViewModel);
	views.push_back(primitiveView.get());
}

ViewController::~ViewController() = default;

void ViewController::onWindowResize(int clientWidth, int clientHeight)
{
	ClientSizeModel clientSizeModel;
	clientSizeModel.clientWidth = clientWidth;
	clientSizeModel.clientHeight = clientHeight;
	mainPassModelsListener.onClientSizeUpdated(clientSizeModel);
}

void ViewController::onMouseDown(int x, int y)
{
	lastMousePosition.x = x;
	lastMousePosition.y = y;
}

void ViewController::onMouseMove(int btnState, int x, int y)
{
	//if (btnState == 0)
	//{
	//	// Make each pixel correspond to a quarter of a degree.
	//	float dx = XMConvertToRadians(0.25f * static_cast<float>(x - lastMousePosition.x));
	//	float dy = XMConvertToRadians(0.25f * static_cast<float>(y - lastMousePosition.y));

	//	// Update angles based on input to orbit camera around box.
	//	cameraViewModel.theta += dx;
	//	cameraViewModel.phi += dy;

	//	// Restrict the angle mPhi.
	//	cameraViewModel.phi = MathHelper::Clamp(cameraViewModel.phi, 0.1f, MathHelper::Pi - 0.1f);
	//}
	//else if (btnState == 1)
	//{
	//	// Make each pixel correspond to 0.2 unit in the scene.
	//	float dx = 0.2f * static_cast<float>(x - lastMousePosition.x);
	//	float dy = 0.2f * static_cast<float>(y - lastMousePosition.y);

	//	// Update the camera radius based on input.
	//	cameraViewModel.radius += dx - dy;

	//	// Restrict the radius.
	//	cameraViewModel.radius = MathHelper::Clamp(cameraViewModel.radius, 5.0f, 150.0f);
	//}

	//lastMousePosition.x = x;
	//lastMousePosition.y = y;
}

void ViewController::present()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto view : views) {
		view->present();
	}

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ViewController::update()
{
	for (auto view : views) {
		auto &viewModel = view->getViewModel();
		viewModel.update();
	}

	CameraModel model = cameraModel(cameraViewModel.get());
	mainPassModelsListener.onCameraModelUpdated(model);

	LightModel updatedLightModel = lightModel(lightViewModel.get());
	mainPassModelsListener.onLightModelUpdated(updatedLightModel);

	FogModel updatedFogModel = fogModel(fogViewModel.get());
	mainPassModelsListener.onFogModelUpdated(updatedFogModel);
}

CameraModel ViewController::cameraModel(CameraViewModel*cameraViewModel)
{
	CameraModel cameraModel;

	cameraModel.theta = cameraViewModel->theta;
	cameraModel.phi = cameraViewModel->phi;
	cameraModel.radius = cameraViewModel->radius;

	return cameraModel;
}

LightModel ViewController::lightModel(LightViewModel* lightViewModel)
{
	LightModel lightModel;

	lightModel.ambientLight = DirectX::XMFLOAT4(lightViewModel->ambient[0], lightViewModel->ambient[1], lightViewModel->ambient[2], lightViewModel->ambient[3]);
	lightModel.direction = DirectX::XMFLOAT3(lightViewModel->direction[0], lightViewModel->direction[1], lightViewModel->direction[2]);

	return lightModel;
}

FogModel ViewController::fogModel(FogViewModel* fogViewModel)
{
	FogModel fogModel;

	fogModel.color = DirectX::XMFLOAT4(fogViewModel->color.x, fogViewModel->color.y, fogViewModel->color.z, fogViewModel->color.w);
	fogModel.start = fogViewModel->start;
	fogModel.range = fogViewModel->range;

	return fogModel;
}

//PrimitiveModel ViewController::primitiveModel(PrimitiveViewModel primitiveViewModel)
//{
//	PrimitiveModel primitiveModel;
//
//	primitiveModel.mesh = primitiveViewModel.currentMesh;
//	primitiveModel.submesh = primitiveViewModel.currentSubMesh;
//	primitiveModel.material = primitiveViewModel.currentMaterial;
//	primitiveModel.position = std::vector<float>(std::begin(primitiveViewModel.position), std::end(primitiveViewModel.position));
//	primitiveModel.scaling = std::vector<float>(std::begin(primitiveViewModel.scaling), std::end(primitiveViewModel.scaling));
//	primitiveModel.texture = std::vector<float>(std::begin(primitiveViewModel.texture), std::end(primitiveViewModel.texture));
//
//	return primitiveModel;
//}
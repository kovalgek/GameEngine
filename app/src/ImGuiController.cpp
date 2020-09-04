#include "ImGuiController.h"
#include "Application.h"
#include "SrvHeapProvider.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"

#include <string.h>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

#include <iostream>

using namespace DirectX;

ImGuiController::ImGuiController(
	HWND                      mainWindowHandle,
	ID3D12Device              *device,
	ID3D12GraphicsCommandList *commandList,
	SrvHeapProvider           *srvHeapProvider,
	MainPassDataProvider      *mainPassDataProvider,
	ObjectsDataProvider       *objectsDataProvider,
	MaterialsDataProvider     *materialsDataProvider,
	GeometryStorage           *geometryStorage) :

	device { device },
	commandList { commandList },
	mainPassDataProvider { mainPassDataProvider },
	objectsDataProvider { objectsDataProvider },
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
		srvHeapProvider->getSrvDescriptorHeap(),
		srvHeapProvider->getSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		srvHeapProvider->getSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	ImGui::StyleColorsDark();

	initPrimitiveViewModel(primitiveViewModel);
}

void ImGuiController::initPrimitiveViewModel(PrimitiveViewModel& primitiveViewModel)
{
	primitiveViewModel.scaling[0] = 1.0f;
	primitiveViewModel.scaling[1] = 1.0f;
	primitiveViewModel.scaling[2] = 1.0f;

	primitiveViewModel.texture[0] = 1.0f;
	primitiveViewModel.texture[1] = 1.0f;
	primitiveViewModel.texture[2] = 1.0f;

	meshes = geometryStorage->getGeometryNames();
	primitiveViewModel.currentMesh = meshes.begin()->first;

	auto submeshes = meshes.begin()->second;
	primitiveViewModel.currentSubMesh = submeshes[0];

	materials = materialsDataProvider->getMaterialNames();
	primitiveViewModel.currentMaterial = materials[0];
}

void ImGuiController::present()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	createCameraView();
	createLightView();
	createPrimitiveFactoryView();

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiController::update()
{
	CameraModel model = cameraModel(cameraViewModel);
	mainPassDataProvider->updateCameraModel(model);

	LightModel updatedLightModel = lightModel(lightsViewModel);
	mainPassDataProvider->updateLight(updatedLightModel);
}

void ImGuiController::createCameraView()
{
	ImGui::Begin("Camera");
	ImGui::SliderFloat("theta", &cameraViewModel.theta, 0.0f, XM_2PI);
	ImGui::SliderFloat("phi", &cameraViewModel.phi, 0.0f, XM_PI);
	ImGui::SliderFloat("radius", &cameraViewModel.radius, 0.0f, 150.0f);
	ImGui::End();
}

void ImGuiController::createLightView()
{
	ImGui::Begin("Light");
	ImGui::InputFloat4("ambient", lightsViewModel.ambient);
	ImGui::InputFloat3("direction", lightsViewModel.direction);
	ImGui::End();
}

void ImGuiController::createPrimitiveFactoryView()
{
	ImGui::Begin("Factory");

	createMeshCombo();
	createSubmeshCombo();
	createMaterialCombo();

	ImGui::InputFloat3("position", primitiveViewModel.position);
	ImGui::InputFloat3("scaling", primitiveViewModel.scaling);
	ImGui::InputFloat3("texture", primitiveViewModel.texture);

	if (ImGui::Button("Create primitive"))
	{
		auto model = primitiveModel(primitiveViewModel);
		objectsDataProvider->createPrimitive(model);
	}

	ImGui::End();
}

void ImGuiController::createMeshCombo()
{
	if (ImGui::BeginCombo("mesh", primitiveViewModel.currentMesh.c_str()))
	{
		for (auto mesh : meshes)
		{
			bool is_selected = primitiveViewModel.currentMesh == mesh.first;
			if (ImGui::Selectable(mesh.first.c_str(), is_selected))
			{
				primitiveViewModel.currentMesh = mesh.first;
				primitiveViewModel.currentSubMesh = mesh.second[0];
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void ImGuiController::createSubmeshCombo()
{
	if (ImGui::BeginCombo("submesh", primitiveViewModel.currentSubMesh.c_str()))
	{
		for (auto submesh : meshes[primitiveViewModel.currentMesh])
		{
			bool is_selected = primitiveViewModel.currentSubMesh == submesh;
			if (ImGui::Selectable(submesh.c_str(), is_selected))
			{
				primitiveViewModel.currentSubMesh = submesh;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void ImGuiController::createMaterialCombo()
{
	if (ImGui::BeginCombo("materials", primitiveViewModel.currentMaterial.c_str()))
	{
		for (auto material : materials)
		{
			bool is_selected = primitiveViewModel.currentMaterial == material;
			if (ImGui::Selectable(material.c_str(), is_selected))
			{
				primitiveViewModel.currentMaterial = material;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

CameraModel ImGuiController::cameraModel(CameraViewModel cameraViewModel)
{
	CameraModel cameraModel;

	cameraModel.theta = cameraViewModel.theta;
	cameraModel.phi = cameraViewModel.phi;
	cameraModel.radius = cameraViewModel.radius;

	return cameraModel;
}

LightModel ImGuiController::lightModel(LightsViewModel lightsViewModel)
{
	LightModel lightModel;

	lightModel.ambientLight = DirectX::XMFLOAT4(lightsViewModel.ambient[0], lightsViewModel.ambient[1], lightsViewModel.ambient[2], lightsViewModel.ambient[3]);
	lightModel.direction = DirectX::XMFLOAT3(lightsViewModel.direction[0], lightsViewModel.direction[1], lightsViewModel.direction[2]);

	return lightModel;
}

PrimitiveModel ImGuiController::primitiveModel(PrimitiveViewModel primitiveViewModel)
{
	PrimitiveModel primitiveModel;

	primitiveModel.mesh = primitiveViewModel.currentMesh;
	primitiveModel.submesh = primitiveViewModel.currentSubMesh;
	primitiveModel.material = primitiveViewModel.currentMaterial;
	primitiveModel.position = std::vector<float>(std::begin(primitiveViewModel.position), std::end(primitiveViewModel.position));
	primitiveModel.scaling = std::vector<float>(std::begin(primitiveViewModel.scaling), std::end(primitiveViewModel.scaling));
	primitiveModel.texture = std::vector<float>(std::begin(primitiveViewModel.texture), std::end(primitiveViewModel.texture));

	return primitiveModel;
}
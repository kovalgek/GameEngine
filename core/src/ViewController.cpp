#include "ViewController.h"
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

ViewController::ViewController(
	HWND                      mainWindowHandle,
	ID3D12Device              *device,
	ID3D12GraphicsCommandList *commandList,
	SrvHeapProvider           *srvHeapProvider,
	MainPassModelsListener    *mainPassModelsListener,
	ObjectsDataProvider       *objectsDataProvider,
	MaterialsDataProvider     *materialsDataProvider,
	GeometryStorage           *geometryStorage) :

	device { device },
	commandList { commandList },
	mainPassModelsListener{ mainPassModelsListener },
	objectsDataProvider { objectsDataProvider },
	materialsDataProvider { materialsDataProvider },
	geometryStorage { geometryStorage }
{
	initPrimitiveViewModel(primitiveViewModel);
	initLightsViewModel(lightsViewModel);

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

}

void ViewController::onWindowResize(int clientWidth, int clientHeight)
{
	ClientSizeModel clientSizeModel;
	clientSizeModel.clientWidth = clientWidth;
	clientSizeModel.clientHeight = clientHeight;
	mainPassModelsListener->onClientSizeUpdated(clientSizeModel);
}

void ViewController::onMouseDown(int x, int y)
{
	lastMousePosition.x = x;
	lastMousePosition.y = y;
}

void ViewController::onMouseMove(int btnState, int x, int y)
{
	if (btnState == 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - lastMousePosition.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - lastMousePosition.y));

		// Update angles based on input to orbit camera around box.
		cameraViewModel.theta += dx;
		cameraViewModel.phi += dy;

		// Restrict the angle mPhi.
		cameraViewModel.phi = MathHelper::Clamp(cameraViewModel.phi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if (btnState == 1)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f * static_cast<float>(x - lastMousePosition.x);
		float dy = 0.2f * static_cast<float>(y - lastMousePosition.y);

		// Update the camera radius based on input.
		cameraViewModel.radius += dx - dy;

		// Restrict the radius.
		cameraViewModel.radius = MathHelper::Clamp(cameraViewModel.radius, 5.0f, 150.0f);
	}

	lastMousePosition.x = x;
	lastMousePosition.y = y;
}

void ViewController::initLightsViewModel(LightsViewModel& lightsViewModel)
{
	lightsViewModel.ambient[0] = 1;
	lightsViewModel.ambient[1] = 1;
	lightsViewModel.ambient[2] = 1;
	lightsViewModel.ambient[3] = 1;

	lightsViewModel.direction[0] = 0;
	lightsViewModel.direction[1] = 0;
	lightsViewModel.direction[2] = 0;

	lightsViewModel.strength[0] = 0;
	lightsViewModel.strength[1] = 0;
	lightsViewModel.strength[2] = 0;
}

void ViewController::initPrimitiveViewModel(PrimitiveViewModel& primitiveViewModel)
{
	primitiveViewModel.position[0] = 0.0f;
	primitiveViewModel.position[1] = 0.0f;
	primitiveViewModel.position[2] = 0.0f;

	primitiveViewModel.scaling[0] = 1.0f;
	primitiveViewModel.scaling[1] = 1.0f;
	primitiveViewModel.scaling[2] = 1.0f;

	primitiveViewModel.texture[0] = 1.0f;
	primitiveViewModel.texture[1] = 1.0f;
	primitiveViewModel.texture[2] = 1.0f;

	meshes = geometryStorage->getGeometryNames();
	if (meshes.size() != 0)
	{
		auto firstMesh = meshes.begin();
		primitiveViewModel.currentMesh = firstMesh->first;

		auto submeshes = firstMesh->second;
		if (submeshes.size() != 0)
		{
			primitiveViewModel.currentSubMesh = submeshes[0];
		}
	}

	materials = materialsDataProvider->getMaterialNames();
	if (materials.size() != 0)
	{
		primitiveViewModel.currentMaterial = materials[0];
	}
}

void ViewController::present()
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

void ViewController::update()
{
	CameraModel model = cameraModel(cameraViewModel);
	mainPassModelsListener->onCameraModelUpdated(model);

	LightModel updatedLightModel = lightModel(lightsViewModel);
	mainPassModelsListener->onLightModelUpdated(updatedLightModel);
}

void ViewController::createCameraView()
{
	ImGui::Begin("Camera");
	ImGui::SliderFloat("theta", &cameraViewModel.theta, 0.0f, XM_2PI);
	ImGui::SliderFloat("phi", &cameraViewModel.phi, 0.0f, XM_PI);
	ImGui::SliderFloat("radius", &cameraViewModel.radius, 0.0f, 150.0f);
	ImGui::End();
}

void ViewController::createLightView()
{
	ImGui::Begin("Light");
	ImGui::InputFloat4("ambient", lightsViewModel.ambient);
	ImGui::InputFloat3("direction", lightsViewModel.direction);
	ImGui::End();
}

void ViewController::createPrimitiveFactoryView()
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

void ViewController::createMeshCombo()
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

void ViewController::createSubmeshCombo()
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

void ViewController::createMaterialCombo()
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

CameraModel ViewController::cameraModel(CameraViewModel cameraViewModel)
{
	CameraModel cameraModel;

	cameraModel.theta = cameraViewModel.theta;
	cameraModel.phi = cameraViewModel.phi;
	cameraModel.radius = cameraViewModel.radius;

	return cameraModel;
}

LightModel ViewController::lightModel(LightsViewModel lightsViewModel)
{
	LightModel lightModel;

	lightModel.ambientLight = DirectX::XMFLOAT4(lightsViewModel.ambient[0], lightsViewModel.ambient[1], lightsViewModel.ambient[2], lightsViewModel.ambient[3]);
	lightModel.direction = DirectX::XMFLOAT3(lightsViewModel.direction[0], lightsViewModel.direction[1], lightsViewModel.direction[2]);

	return lightModel;
}

PrimitiveModel ViewController::primitiveModel(PrimitiveViewModel primitiveViewModel)
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
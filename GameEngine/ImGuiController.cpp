#include "ImGuiController.h"
#include "Application.h"
#include "SrvHeapProvider.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"

#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx12.h"

using namespace DirectX;

ImGuiController::ImGuiController(
	Application* application,
	SrvHeapProvider *srvHeapProvider,
	HWND mainWindowHandle,
	MainPassDataProvider* mainPassDataProvider,
	ObjectsDataProvider *objectsDataProvider) :
	application { application },
	mainPassDataProvider { mainPassDataProvider },
	objectsDataProvider{ objectsDataProvider }
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(mainWindowHandle);
	ImGui_ImplDX12_Init(application->getDevice(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		srvHeapProvider->getSrvDescriptorHeap(),
		srvHeapProvider->getSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		srvHeapProvider->getSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	ImGui::StyleColorsDark();
}

void ImGuiController::present()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Camera");
	ImGui::SliderFloat("theta", &theta, 0.0f, XM_2PI);
	ImGui::SliderFloat("phi", &phi, 0.0f, XM_PI);
	ImGui::SliderFloat("radius", &radius, 0.0f, 150.0f);
	ImGui::End();

	ImGui::Begin("Factory");
	ImGui::InputFloat3("hills coords", hillsCoords);
	if (ImGui::Button("Create hills"))
	{
		objectsDataProvider->createPrimitive("hills", "grass", { hillsCoords[0], hillsCoords[1], hillsCoords[2], }, { 1.0f, 1.0f, 1.0f, }, { 5.0f, 5.0f, 1.0f, });
	}
	ImGui::InputFloat3("box coords", boxCoords);
	if (ImGui::Button("Create box"))
	{
		objectsDataProvider->createPrimitive("box", "wirefence", { boxCoords[0], boxCoords[1], boxCoords[2], }, { 1.0f, 1.0f, 1.0f, }, { 1.0f, 1.0f, 1.0f, });
	}
	//ImGui::InputFloat3("sphere coords", sphereCoords);
	//if (ImGui::Button("Create sphere"))
	//{
	//	objectsDataProvider->createPrimitive("sphere", "wirefence", { sphereCoords[0], sphereCoords[1], sphereCoords[2], }, { 1.0f, 1.0f, 1.0f, }, { 1.0f, 1.0f, 1.0f, });
	//}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), application->getCommandList());

	mainPassDataProvider->updateData(theta, phi, radius);
}
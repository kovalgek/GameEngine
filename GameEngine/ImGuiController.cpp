#include "ImGuiController.h"
#include "Application.h"
#include "TexturesController.h"

#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx12.h"


ImGuiController::ImGuiController(Application* application, TexturesController *texturesController, HWND mainWindowHandle) : application { application }
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(mainWindowHandle);
	ImGui_ImplDX12_Init(application->getDevice(), 3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		texturesController->getSrvDescriptorHeap(),
		texturesController->getSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		texturesController->getSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	ImGui::StyleColorsDark();
}

void ImGuiController::present()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Hello, world!");
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), application->getCommandList());
}
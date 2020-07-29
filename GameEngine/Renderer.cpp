#include "Renderer.h"
#include "Application.h"
#include "d3dx12.h"
#include "d3dUtil.h"
#include "UploadBuffer.h"
#include "GameTimer.h"
#include "FrameResourceController.h"
#include "PipleneStateData.h"
#include "ObjectsDataProvider.h"
#include "RenderItem.h"
#include "Material.h"
#include "FrameResource.h"
#include "SrvHeapProvider.h"
#include "ImGuiController.h"

#include <DirectXColors.h>
#include <DirectXPackedVector.h>
#include <D3Dcompiler.h>
#include <array>


using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

Renderer::Renderer(
	Application* appContext,
	std::unique_ptr<PipleneStateData> pipleneStateData,
	FrameResourceController* frameResourceController,
	ObjectsDataProvider* objectsDataProvider,
	std::unique_ptr <SrvHeapProvider> srvHeapProvider,
	std::unique_ptr <ImGuiController> imGuiController) :

	appContext{ appContext },
	pipleneStateData { std::move(pipleneStateData) },
	frameResourceController { frameResourceController },
	objectsDataProvider { objectsDataProvider },
	srvHeapProvider{ std::move(srvHeapProvider) },
	imGuiController { std::move(imGuiController) },
	device { appContext->getDevice() },
	commandQueue { appContext->getCommandQueue() },
	commandList { appContext->getCommandList() },
	commandAllocator { appContext->getCommandAllocator() }
{
	//// Setup Dear ImGui context
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();

	//ImGui_ImplWin32_Init(mainWindowHandle);
	//ImGui_ImplDX12_Init(device, 3,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	this->texturesController->getSrvDescriptorHeap(),
	//	this->texturesController->getSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
	//	this->texturesController->getSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	//ImGui::StyleColorsDark();
}

Renderer::~Renderer()
{

}

void Renderer::onKeyboardInput(const GameTimer& gameTimer)
{
	if (GetAsyncKeyState('1') & 0x8000)
		isWireframe = true;
	else
		isWireframe = false;
}
bool show_demo_window = true;

void Renderer::draw(const GameTimer& gameTimer)
{


	auto cmdListAlloc = frameResourceController->getCurrentFrameResource()->CmdListAlloc;


	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	//if (isWireframe)
	//{
	//	ThrowIfFailed(commandList->Reset(cmdListAlloc.Get(), pipleneStateData->getPSO("opaque_wireframe")));
	//}
	//else
	//{
		ThrowIfFailed(commandList->Reset(cmdListAlloc.Get(), pipleneStateData->getPSO("opaque")));
//	}

	commandList->RSSetViewports(1, &appContext->getScreenViewport());
	commandList->RSSetScissorRects(1, &appContext->getScissorRect());

	// Indicate a state transition on the resource usage.
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(appContext->currentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	commandList->ClearRenderTargetView(appContext->currentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	commandList->ClearDepthStencilView(appContext->depthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	commandList->OMSetRenderTargets(1, &appContext->currentBackBufferView(), true, &appContext->depthStencilView());

	//ID3D12DescriptorHeap* descriptorHeaps[] = { frameResourceController->getCbvHeap() };
	//commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);



	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeapProvider->getSrvDescriptorHeap()};
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	commandList->SetGraphicsRootSignature(pipleneStateData->getRootSignature());

	//commandList->SetGraphicsRootDescriptorTable(1, frameResourceController->passCbvHandle());

	auto passCB = frameResourceController->getCurrentFrameResource()->PassCB->Resource();
	commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	auto allRitems = objectsDataProvider->renderItemsForLayer(RenderLayer::Opaque);
	drawRenderItems(commandList, allRitems);

	imGuiController->present();

	// Indicate a state transition on the resource usage.
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(appContext->currentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));




	// Done recording commands.
	ThrowIfFailed(commandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);



	appContext->swapChainMethod();

	//// Advance the fence value to mark commands up to this fence point.
	frameResourceController->getCurrentFrameResource()->Fence = ++appContext->currentFence;

	//// Add an instruction to the command queue to set a new fence point. 
	//// Because we are on the GPU timeline, the new fence point won't be 
	//// set until the GPU finishes processing all the commands prior to this Signal().
	commandQueue->Signal(appContext->getFence(), appContext->currentFence);
}

void Renderer::drawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = frameResourceController->getCurrentFrameResource()->ObjectCB->Resource();
	auto matCB = frameResourceController->getCurrentFrameResource()->MaterialCB->Resource();

	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		auto tex = srvHeapProvider->getHandleForIndex(ri->Mat->DiffuseSrvHeapIndex);
		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootDescriptorTable(0, tex);
		cmdList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}



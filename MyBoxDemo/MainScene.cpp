#include "MainScene.h"
#include "ApplicationContext.h"
#include "d3dUtil.h"
#include "UploadBuffer.h"
#include "d3dx12.h"
#include <DirectXColors.h>
#include <array>
#include <D3Dcompiler.h>

MainScene::MainScene(ApplicationContext *appContext)
{
    this->appContext = appContext;

	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(appContext->getCommandList()->Reset(appContext->getCommandAllocator().Get(), nullptr));

	buildDescriptorHeaps();
    buildConstantBuffers();
	buildRootSignature();
	buildShadersAndInputLayout();
	buildBoxGeometry();
	buildPSO();

	// Execute the initialization commands.
	ThrowIfFailed(appContext->getCommandList()->Close());
	ID3D12CommandList* cmdsLists[] = { appContext->getCommandList().Get() };
	appContext->getCommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	appContext->flushCommandQueue();
}

void MainScene::buildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(appContext->getDevice()->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvHeap)));
}

void MainScene::buildConstantBuffers()
{
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(appContext->getDevice().Get(), 1, true);

	UINT objCBByteSize = d3dUtil::calcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::calcConstantBufferByteSize(sizeof(ObjectConstants));

	appContext->getDevice()->CreateConstantBufferView(
		&cbvDesc,
		cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void MainScene::buildRootSignature()
{
	// Shader programs typically require resources as input (constant buffers,
	// textures, samplers).  The root signature defines the resources the shader
	// programs expect.  If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be
	// thought of as defining the function signature.  

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(appContext->getDevice()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)));
}

void MainScene::buildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	vsByteCode = d3dUtil::compileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	psByteCode = d3dUtil::compileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void MainScene::buildBoxGeometry()
{
	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	boxGeo = std::make_unique<MeshGeometry>();
	boxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &boxGeo->VertexBufferCPU));
	CopyMemory(boxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &boxGeo->IndexBufferCPU));
	CopyMemory(boxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	boxGeo->VertexBufferGPU = d3dUtil::createDefaultBuffer(appContext->getDevice().Get(),
		appContext->getCommandList().Get(), vertices.data(), vbByteSize, boxGeo->VertexBufferUploader);

	boxGeo->IndexBufferGPU = d3dUtil::createDefaultBuffer(appContext->getDevice().Get(),
		appContext->getCommandList().Get(), indices.data(), ibByteSize, boxGeo->IndexBufferUploader);

	boxGeo->VertexByteStride = sizeof(Vertex);
	boxGeo->VertexBufferByteSize = vbByteSize;
	boxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	boxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	boxGeo->DrawArgs["box"] = submesh;
}

void MainScene::buildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(vsByteCode->GetBufferPointer()),
		vsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(psByteCode->GetBufferPointer()),
		psByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = appContext->getBackBufferFormat();
	psoDesc.SampleDesc.Count = appContext->getMsaa4xState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = appContext->getMsaa4xState() ? (appContext->getMsaa4xQuality() - 1) : 0;
	psoDesc.DSVFormat = appContext->getDepthStencilFormat();
	ThrowIfFailed(appContext->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PSO)));
}

void MainScene::onResize(int clientWidth, int clientHeight)
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	float aspectRatio = static_cast<float>(clientWidth) / clientHeight;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}


void MainScene::update(const GameTimer& gameTimer)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	// Update the constant buffer with the latest worldViewProj matrix.
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	mObjectCB->CopyData(0, objConstants);
}

void MainScene::draw(const GameTimer& gameTimer)
{
	// Reuse the memory associated with command recording.
 // We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(appContext->getCommandAllocator()->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(appContext->getCommandList()->Reset(appContext->getCommandAllocator().Get(), PSO.Get()));

	appContext->getCommandList()->RSSetViewports(1, &appContext->getScreenViewport());
	appContext->getCommandList()->RSSetScissorRects(1, &appContext->getScissorRect());

	// Indicate a state transition on the resource usage.
	appContext->getCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(appContext->currentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Clear the back buffer and depth buffer.
	auto currentBackBufferView = appContext->currentBackBufferView();
	appContext->getCommandList()->ClearRenderTargetView(appContext->currentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	appContext->getCommandList()->ClearDepthStencilView(appContext->depthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	appContext->getCommandList()->OMSetRenderTargets(1, &appContext->currentBackBufferView(), true, &appContext->depthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { cbvHeap.Get() };
	appContext->getCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	appContext->getCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	appContext->getCommandList()->IASetVertexBuffers(0, 1, &boxGeo->VertexBufferView());
	appContext->getCommandList()->IASetIndexBuffer(&boxGeo->IndexBufferView());
	appContext->getCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto qwe = cbvHeap->GetGPUDescriptorHandleForHeapStart();
	appContext->getCommandList()->SetGraphicsRootDescriptorTable(0, cbvHeap->GetGPUDescriptorHandleForHeapStart());

	appContext->getCommandList()->DrawIndexedInstanced(
		boxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);

	// Indicate a state transition on the resource usage.
	appContext->getCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(appContext->currentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(appContext->getCommandList()->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { appContext->getCommandList().Get() };
	appContext->getCommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(appContext->swapChain->Present(0, 0));
	appContext->currBackBuffer = (appContext->currBackBuffer + 1) % ApplicationContext::swapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	appContext->flushCommandQueue();
}
#include "PipleneStateData.h"
#include "d3dUtil.h"
#include "d3dx12.h"
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

PipleneStateData::PipleneStateData(
	ID3D12Device* device,
	DXGI_FORMAT backBufferFormat,
	DXGI_FORMAT depthStencilFormat,
	bool msaa4xState,
	UINT msaa4xQuality
) : 
	device{ device }, 
	backBufferFormat{ backBufferFormat },
	depthStencilFormat { depthStencilFormat },
	msaa4xState { msaa4xState },
	msaa4xQuality { msaa4xQuality }
{
	buildShadersAndInputLayout();
	buildRootSignature();
	buildPSOs();
}

void PipleneStateData::buildShadersAndInputLayout()
{
	shaders["standardVS"] = d3dUtil::compileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
	shaders["opaquePS"] = d3dUtil::compileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");

	inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void PipleneStateData::buildRootSignature()
{
	//CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	//cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	//CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	//cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	//// Root parameter can be a table, root descriptor or root constants.
	//CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	//// Create root CBVs.
	//slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	//slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Create root CBV.
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr,
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

	ThrowIfFailed(device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(rootSignature.GetAddressOf())));
}

void PipleneStateData::buildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	opaquePsoDesc.pRootSignature = rootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(shaders["standardVS"]->GetBufferPointer()),
		shaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(shaders["opaquePS"]->GetBufferPointer()),
		shaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = backBufferFormat;
	opaquePsoDesc.SampleDesc.Count = msaa4xState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = msaa4xState ? (msaa4xQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = depthStencilFormat;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));


	//
	// PSO for opaque wireframe objects.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
}

ID3D12PipelineState* PipleneStateData::getPSO(std::string name)
{
	return mPSOs[name].Get();
}
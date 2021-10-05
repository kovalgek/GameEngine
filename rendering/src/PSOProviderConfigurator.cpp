#include "PSOProviderConfigurator.h"
#include "d3dx12.h"
#include "d3dUtil.h"
#include "PSOProvider.h"

using Microsoft::WRL::ComPtr;

PSOProviderConfigurator::PSOProviderConfigurator(
	ID3D12Device* device,
	ID3D12RootSignature* rootSignature,
	DXGI_FORMAT backBufferFormat,
	DXGI_FORMAT depthStencilFormat,
	bool msaa4xState,
	UINT msaa4xQuality
) : 
	device{ device },
	rootSignature { rootSignature },
	backBufferFormat{ backBufferFormat },
	depthStencilFormat{ depthStencilFormat },
	msaa4xState{ msaa4xState },
	msaa4xQuality{ msaa4xQuality }
{
	inputLayout = createInputLayout();

	shaders["standardVS"] = standardVertexShader();
	shaders["opaquePS"] = opaquePixelShader();
	shaders["alphaTestedPS"] = alphaTestedPixelShader();
}

void PSOProviderConfigurator::configure(PSOProvider& psoProvider)
{
	ComPtr<ID3D12PipelineState> pipelineState;
	createOpaquePSO(pipelineState.GetAddressOf());
	psoProvider.addPipelineStateObject("opaque", pipelineState);

	ComPtr<ID3D12PipelineState> transparentPipelineState;
	createTransparentPSO(transparentPipelineState.GetAddressOf());
	psoProvider.addPipelineStateObject("transparent", transparentPipelineState);

	ComPtr<ID3D12PipelineState> alphaTestedPipelineState;
	createAlphaTestedPSO(alphaTestedPipelineState.GetAddressOf());
	psoProvider.addPipelineStateObject("alphaTested", alphaTestedPipelineState);

	ComPtr<ID3D12PipelineState> stencilMirrorsPipelineState;
	createStencilMirrorsPSO(stencilMirrorsPipelineState.GetAddressOf());
	psoProvider.addPipelineStateObject("markStencilMirrors", stencilMirrorsPipelineState);

	ComPtr<ID3D12PipelineState> stencilReflectionsPipelineState;
	createStencilReflectionsPSO(stencilReflectionsPipelineState.GetAddressOf());
	psoProvider.addPipelineStateObject("drawStencilReflections", stencilReflectionsPipelineState);

	ComPtr<ID3D12PipelineState> shadowObjectsPipelineState;
	createShadowObjectsPSO(shadowObjectsPipelineState.GetAddressOf());
	psoProvider.addPipelineStateObject("shadow", shadowObjectsPipelineState);
}

ComPtr<ID3DBlob> PSOProviderConfigurator::standardVertexShader()
{
	return d3dUtil::compileShader(L"..\\core\\resources\\shaders\\Default.hlsl", nullptr, "VS", "vs_5_0");
}

ComPtr<ID3DBlob> PSOProviderConfigurator::opaquePixelShader()
{
	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", NULL,
		NULL, NULL
	};
	return d3dUtil::compileShader(L"..\\core\\resources\\shaders\\Default.hlsl", defines, "PS", "ps_5_0");
}

ComPtr<ID3DBlob> PSOProviderConfigurator::alphaTestedPixelShader()
{
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};
	return d3dUtil::compileShader(L"..\\core\\resources\\shaders\\Default.hlsl", alphaTestDefines, "PS", "ps_5_0");
}

std::vector<D3D12_INPUT_ELEMENT_DESC> PSOProviderConfigurator::createInputLayout()
{
	return {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void PSOProviderConfigurator::createOpaquePSO(ID3D12PipelineState** pipelineState)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePso = opaquePsoDesc();
	ThrowIfFailed(device->CreateGraphicsPipelineState(&opaquePso, IID_PPV_ARGS(pipelineState)));
}

void PSOProviderConfigurator::createTransparentPSO(ID3D12PipelineState** pipelineState)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = transparentPSODesc();
	ThrowIfFailed(device->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(pipelineState)));
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC PSOProviderConfigurator::opaquePsoDesc()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	opaquePsoDesc.pRootSignature = rootSignature;
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
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = backBufferFormat;
	opaquePsoDesc.SampleDesc.Count = msaa4xState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = msaa4xState ? (msaa4xQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = depthStencilFormat;

	return opaquePsoDesc;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC PSOProviderConfigurator::transparentPSODesc()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc();

	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;

	return transparentPsoDesc;
}

void PSOProviderConfigurator::createAlphaTestedPSO(ID3D12PipelineState** pipelineState)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestedPsoDesc = opaquePsoDesc();
	alphaTestedPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(shaders["alphaTestedPS"]->GetBufferPointer()),
		shaders["alphaTestedPS"]->GetBufferSize()
	};
	alphaTestedPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&alphaTestedPsoDesc, IID_PPV_ARGS(pipelineState)));
}

void PSOProviderConfigurator::createStencilMirrorsPSO(ID3D12PipelineState** pipelineState)
{
	CD3DX12_BLEND_DESC mirrorBlendState(D3D12_DEFAULT);
	mirrorBlendState.RenderTarget[0].RenderTargetWriteMask = 0;

	D3D12_DEPTH_STENCIL_DESC mirrorDSS;
	mirrorDSS.DepthEnable = true;
	mirrorDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	mirrorDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	mirrorDSS.StencilEnable = true;
	mirrorDSS.StencilReadMask = 0xff;
	mirrorDSS.StencilWriteMask = 0xff;

	mirrorDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	mirrorDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	// We are not rendering backfacing polygons, so these settings do not matter.
	mirrorDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	mirrorDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC markMirrorsPsoDesc = opaquePsoDesc();
	markMirrorsPsoDesc.BlendState = mirrorBlendState;
	markMirrorsPsoDesc.DepthStencilState = mirrorDSS;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&markMirrorsPsoDesc, IID_PPV_ARGS(pipelineState)));
}

void PSOProviderConfigurator::createStencilReflectionsPSO(ID3D12PipelineState** pipelineState)
{
	D3D12_DEPTH_STENCIL_DESC reflectionsDSS;
	reflectionsDSS.DepthEnable = true;
	reflectionsDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	reflectionsDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	reflectionsDSS.StencilEnable = true;
	reflectionsDSS.StencilReadMask = 0xff;
	reflectionsDSS.StencilWriteMask = 0xff;

	reflectionsDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	reflectionsDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC drawReflectionsPsoDesc = opaquePsoDesc();
	drawReflectionsPsoDesc.DepthStencilState = reflectionsDSS;
	drawReflectionsPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	drawReflectionsPsoDesc.RasterizerState.FrontCounterClockwise = true;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&drawReflectionsPsoDesc, IID_PPV_ARGS(pipelineState)));
}

void PSOProviderConfigurator::createShadowObjectsPSO(ID3D12PipelineState** pipelineState)
{
	// We are going to draw shadows with transparency, so base it off the transparency description.
	D3D12_DEPTH_STENCIL_DESC shadowDSS;
	shadowDSS.DepthEnable = true;
	shadowDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	shadowDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	shadowDSS.StencilEnable = true;
	shadowDSS.StencilReadMask = 0xff;
	shadowDSS.StencilWriteMask = 0xff;

	shadowDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	shadowDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	shadowDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	shadowDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	shadowDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	shadowDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	shadowDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	shadowDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowPsoDesc = transparentPSODesc();
	shadowPsoDesc.DepthStencilState = shadowDSS;
	ThrowIfFailed(device->CreateGraphicsPipelineState(&shadowPsoDesc, IID_PPV_ARGS(pipelineState)));
}
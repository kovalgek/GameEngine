#include <wrl/client.h>
#include <unordered_map>
#include "d3dx12.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct D3D12_INPUT_ELEMENT_DESC;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
enum DXGI_FORMAT;
class PSOProvider;
struct D3D12_GRAPHICS_PIPELINE_STATE_DESC;

#pragma once
class PSOProviderConfigurator
{
public:
	PSOProviderConfigurator(
		ID3D12Device* device,
		ID3D12RootSignature* rootSignature,
		DXGI_FORMAT backBufferFormat,
		DXGI_FORMAT depthStencilFormat,
		bool msaa4xState,
		UINT msaa4xQuality
	);
	~PSOProviderConfigurator() = default;

	void configure(PSOProvider& psoProvider);

private:

	ID3D12Device* device;
	ID3D12RootSignature* rootSignature;
	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilFormat;
	bool msaa4xState;
	UINT msaa4xQuality;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	std::vector<D3D12_INPUT_ELEMENT_DESC> createInputLayout();

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> shaders;
	Microsoft::WRL::ComPtr<ID3DBlob> standardVertexShader();
	Microsoft::WRL::ComPtr<ID3DBlob> opaquePixelShader();
	Microsoft::WRL::ComPtr<ID3DBlob> alphaTestedPixelShader();

	void createOpaquePSO(ID3D12PipelineState** pipelineState);
	void createTransparentPSO(ID3D12PipelineState** pipelineState);
	void createAlphaTestedPSO(ID3D12PipelineState** pipelineState);
	void createStencilMirrorsPSO(ID3D12PipelineState** pipelineState);
	void createStencilReflectionsPSO(ID3D12PipelineState** pipelineState);
	void createShadowObjectsPSO(ID3D12PipelineState** pipelineState);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc();
	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPSODesc();
};


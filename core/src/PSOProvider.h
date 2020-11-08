#include <wrl/client.h>
#include <unordered_map>
#include <d3dx12.h>

#pragma once
class PSOProvider
{
public:
	PSOProvider() = default;
	~PSOProvider() = default;

	void addPipelineStateObject(std::string name, Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject);
	ID3D12PipelineState* getPipelineStateObject(std::string name);

private:
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> pipelineStateObjects;
};


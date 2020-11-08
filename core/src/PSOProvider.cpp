#include "PSOProvider.h"

void PSOProvider::addPipelineStateObject(std::string name, Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObject)
{
	pipelineStateObjects[name] = pipelineStateObject;
}

ID3D12PipelineState* PSOProvider::getPipelineStateObject(std::string name)
{
	return pipelineStateObjects.find(name)->second.Get();
}
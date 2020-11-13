#include "FrameResource.h"
#include "UploadBuffer.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, std::vector<UINT> vertexBufferSizes)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
    ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, 128, true);
    MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);

    for (UINT vertexBufferSize : vertexBufferSizes)
    {
        auto buffer = std::make_unique<UploadBuffer<Vertex>>(device, vertexBufferSize, false);
        vertexBuffers.push_back(std::move(buffer));
    }
}

FrameResource::~FrameResource()
{

}
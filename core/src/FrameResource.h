#pragma once

#include <wrl/client.h>
#include <memory>
#include "PassConstants.h"
#include "ObjectConstants.h"
#include "MaterialConstants.h"
#include "Vertex.h"
#include "d3dUtil.h"

template<typename T> class UploadBuffer;
struct ID3D12Device;
struct ID3D12CommandAllocator;

/**
 * Stores the resources needed for the CPU to build the command lists for a frame.
**/
struct FrameResource
{
public:
    
    FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, std::vector<UINT> vertexBufferSizes);
    FrameResource(const FrameResource& rhs) = delete;
    FrameResource& operator=(const FrameResource& rhs) = delete;
    ~FrameResource();

    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

    // We cannot update a cbuffer until the GPU is done processing the commands
    // that reference it.  So each frame needs their own cbuffers.
    std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
    std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
    std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

    // We cannot update a dynamic vertex buffer until the GPU is done processing
    // the commands that reference it.  So each frame needs their own.
    std::vector<std::unique_ptr<UploadBuffer<Vertex>>> vertexBuffers;

    std::vector<UploadBuffer<Vertex>*> getVertexBuffers()
    {
        std::vector<UploadBuffer<Vertex>*> tempVertexBuffers;

        for (auto& vertexBuffer : vertexBuffers)
        {
            tempVertexBuffers.push_back(vertexBuffer.get());
        }
        return tempVertexBuffers;
    }

    // Fence value to mark commands up to this fence point.  This lets us
    // check if these frame resources are still in use by the GPU.
    UINT64 Fence = 0;
};
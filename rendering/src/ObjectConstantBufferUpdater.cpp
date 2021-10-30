#include "ObjectConstantBufferUpdater.h"
#include "FrameResource.h"
#include "GameTimer.h"
#include "UploadBuffer.h"
#include "ObjectConstants.h"
#include <DirectXMath.h>
#include "Scene.h"

using namespace DirectX;

ObjectConstantBufferUpdater::ObjectConstantBufferUpdater(Scene& scene) : scene { scene }
{

}

void ObjectConstantBufferUpdater::update(FrameResource& frameResource, const GameTimer& gameTimer)
{
	auto objectConstantBuffer = frameResource.ObjectCB.get();	
	auto renderItems = scene.renderComponents();
	updateObjectConstantBuffer(objectConstantBuffer, renderItems);
}

void ObjectConstantBufferUpdater::updateObjectConstantBuffer(UploadBuffer<ObjectConstants>* objectConstantBuffer, std::vector<RenderComponent*> renderComponents)
{
	for (auto renderComponent : renderComponents) {

		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (renderComponent->numberOfDirtyFrames == 0)
		{
			continue;
		}
		// Next FrameResource need to be updated too.
		renderComponent->numberOfDirtyFrames--;

		ObjectConstants objConstants = objectConstantsFromRenderItem(renderComponent);
		objectConstantBuffer->CopyData(renderComponent->objectConstantBufferIndex, objConstants);
	}
}

ObjectConstants ObjectConstantBufferUpdater::objectConstantsFromRenderItem(RenderComponent* renderComponent)
{
	ObjectConstants objConstants;

	XMMATRIX world = XMLoadFloat4x4(&renderComponent->world);
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

	XMMATRIX texTransform = XMLoadFloat4x4(&renderComponent->texture);
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

	return objConstants;
}
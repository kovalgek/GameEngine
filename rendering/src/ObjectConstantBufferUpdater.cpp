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
	auto group = scene.renderAndTransformGroup();
	updateObjectConstantBuffer(objectConstantBuffer, group);
}

void ObjectConstantBufferUpdater::updateObjectConstantBuffer(UploadBuffer<ObjectConstants>* objectConstantBuffer, RenderAndTransformGroup renderAndTransformGroup)
{
	for (auto entity : renderAndTransformGroup) {

		auto& transformComponent = renderAndTransformGroup.get<TransformComponent>(entity);
		auto& renderComponent = renderAndTransformGroup.get<RenderComponent>(entity);

		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (renderComponent.numberOfDirtyFrames == 0)
		{
			continue;
		}
		// Next FrameResource need to be updated too.
		renderComponent.numberOfDirtyFrames--;

		ObjectConstants objConstants = objectConstantsFromRenderItem(&transformComponent);
		objectConstantBuffer->CopyData(renderComponent.objectConstantBufferIndex, objConstants);
	}
}

ObjectConstants ObjectConstantBufferUpdater::objectConstantsFromRenderItem(TransformComponent* renderItem)
{
	ObjectConstants objConstants;

	XMMATRIX world = XMLoadFloat4x4(&renderItem->world);
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

	XMMATRIX texTransform = XMLoadFloat4x4(&renderItem->texture);
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

	return objConstants;
}
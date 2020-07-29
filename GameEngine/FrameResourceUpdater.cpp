#include "FrameResourceUpdater.h"
#include "PassConstants.h"
#include "MaterialConstants.h"
#include "FrameResource.h"
#include "UploadBuffer.h"
#include "FrameResourceController.h"
#include "MainPassDataProvider.h"
#include "ObjectsDataProvider.h"
#include "MaterialsDataProvider.h"
#include "RenderItem.h"
#include "Material.h"
#include "GameTimer.h"
#include "Waves.h"
#include <DirectXColors.h>
#include "Vertex.h"

using namespace DirectX;

FrameResourceUpdater::FrameResourceUpdater(
	std::unique_ptr<FrameResourceController> frameResourceController,
	ID3D12Fence* fence,
	MainPassDataProvider *mainPassDataProvider,
	ObjectsDataProvider *objectsDataProvider,
	MaterialsDataProvider* materialsDataProvider,
	Waves *waves) :

	frameResourceController { std::move(frameResourceController) },
	fence { fence },
	mainPassDataProvider { mainPassDataProvider },
	objectsDataProvider { objectsDataProvider },
	materialsDataProvider { materialsDataProvider },
	waves { waves }
{
}

FrameResourceUpdater::~FrameResourceUpdater() = default;

void FrameResourceUpdater::update(const GameTimer& gameTimer)
{
	frameResourceController->changeFrameResource();

	auto currFrameResource = frameResourceController->getCurrentFrameResource();
	waitForFrameResourceAvailable(currFrameResource);

	updateObjectConstantBufferForFrameResource(currFrameResource);
	updateMaterialConstantBufferForFrameResource(currFrameResource);
	updateMainPassConstantBufferForFrameResource(currFrameResource, gameTimer);
	updateVertexUploadBufferForFrameResource(currFrameResource);
}

void FrameResourceUpdater::waitForFrameResourceAvailable(FrameResource *frameResource)
{
	// Has the GPU finished processing the commands of the current frame resource?
	// If not, wait until the GPU has completed commands up to this fence point.
	if (frameResource->Fence != 0 && fence->GetCompletedValue() < frameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(fence->SetEventOnCompletion(frameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void FrameResourceUpdater::updateMainPassConstantBufferForFrameResource(FrameResource* frameResource, const GameTimer& gameTimer)
{
	auto mainPassConstantBuffer = frameResource->PassCB.get();
	auto mainPassData = mainPassDataProvider->getMainPassData();
	updateMainPassConstantBuffer(mainPassConstantBuffer, mainPassData, gameTimer);
}

void FrameResourceUpdater::updateMainPassConstantBuffer(UploadBuffer<PassConstants>* mainPassConstantBuffer, MainPassData mainPassData, const GameTimer& gameTimer)
{
	PassConstants passConstants = passConstantsFromMainPassData(mainPassData, gameTimer);
	mainPassConstantBuffer->CopyData(0, passConstants);
}

PassConstants FrameResourceUpdater::passConstantsFromMainPassData(MainPassData mainPassData, const GameTimer& gameTimer)
{
	PassConstants mainPassCB;

	XMMATRIX view = XMLoadFloat4x4(&mainPassData.mView);
	XMMATRIX proj = XMLoadFloat4x4(&mainPassData.mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mainPassCB.EyePosW = mainPassData.eyePosition;
	mainPassCB.RenderTargetSize = XMFLOAT2((float)mainPassData.clientWidth, (float)mainPassData.clientHeight);
	mainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mainPassData.clientWidth, 1.0f / mainPassData.clientHeight);
	mainPassCB.NearZ = 1.0f;
	mainPassCB.FarZ = 1000.0f;
	mainPassCB.TotalTime = gameTimer.TotalTime();
	mainPassCB.DeltaTime = gameTimer.DeltaTime();
	mainPassCB.AmbientLight = { mainPassData.ambientLight.x, mainPassData.ambientLight.y, mainPassData.ambientLight.z, mainPassData.ambientLight.w }; //{ 0.25f, 0.25f, 0.35f, 1.0f };
	mainPassCB.Lights[0].Direction = { mainPassData.direction.x, mainPassData.direction.y, mainPassData.direction.z }; // { 0.57735f, -0.57735f, 0.57735f };
	mainPassCB.Lights[0].Strength = { 0.9f, 0.9f, 0.9f };
	mainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mainPassCB.Lights[1].Strength = { 0.5f, 0.5f, 0.5f };
	mainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

	return mainPassCB;
}

void FrameResourceUpdater::updateObjectConstantBufferForFrameResource(FrameResource *frameResource)
{
	auto objectConstantBuffer = frameResource->ObjectCB.get();
	auto renderItems = objectsDataProvider->renderItems();
	updateObjectConstantBuffer(objectConstantBuffer, renderItems);
}

void FrameResourceUpdater::updateObjectConstantBuffer(UploadBuffer<ObjectConstants> * objectConstantBuffer, std::vector<RenderItem*> renderItems)
{
	for (auto& renderItem : renderItems)
	{
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (renderItem->NumFramesDirty == 0)
		{
			continue;
		}
		// Next FrameResource need to be updated too.
		renderItem->NumFramesDirty--;

		ObjectConstants objConstants = objectConstantsFromRenderItem(renderItem);
		objectConstantBuffer->CopyData(renderItem->ObjCBIndex, objConstants);
	}
}

ObjectConstants FrameResourceUpdater::objectConstantsFromRenderItem(RenderItem* renderItem)
{
	ObjectConstants objConstants;

	XMMATRIX world = XMLoadFloat4x4(&renderItem->World);
	XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

	XMMATRIX texTransform = XMLoadFloat4x4(&renderItem->TexTransform);
	XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

	return objConstants;
}

void FrameResourceUpdater::updateMaterialConstantBufferForFrameResource(FrameResource* frameResource)
{
	auto materialConstantBuffer = frameResource->MaterialCB.get();
	auto materials = materialsDataProvider->getMaterials();
	updateMaterialConstantBuffer(materialConstantBuffer, materials);
}

void FrameResourceUpdater::updateMaterialConstantBuffer(UploadBuffer<MaterialConstants>* materialConstantBuffer, std::vector<Material*> materials)
{
	for (Material* material : materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		if (material->NumFramesDirty == 0)
		{
			continue;
		}
		// Next FrameResource need to be updated too.
		material->NumFramesDirty--;
		
		MaterialConstants matConstants = materialConstantsFromMaterial(material);
		materialConstantBuffer->CopyData(material->MatCBIndex, matConstants);
	}
}

MaterialConstants FrameResourceUpdater::materialConstantsFromMaterial(Material* material)
{
	MaterialConstants matConstants;

	XMMATRIX matTransform = XMLoadFloat4x4(&material->MatTransform);
	matConstants.DiffuseAlbedo = material->DiffuseAlbedo;
	matConstants.FresnelR0 = material->FresnelR0;
	matConstants.Roughness = material->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

	return matConstants;
}

void FrameResourceUpdater::updateVertexUploadBufferForFrameResource(FrameResource* frameResource)
{
	auto vertexBuffer = frameResourceController->getCurrentFrameResource()->WavesVB.get();
	auto renderItems = objectsDataProvider->getRenderItemsWithDynamicVertexBuffer();
	updateVertexUploadBuffer(vertexBuffer, renderItems);
}

void FrameResourceUpdater::updateVertexUploadBuffer(UploadBuffer<Vertex>* vertexBuffer, std::vector<RenderItem*> renderItems)
{
	for (auto& renderItem : renderItems)
	{
		auto dynamicVertices = renderItem->dynamicVertices;
		std::vector<Vertex> verticies = dynamicVertices->getVertices();

		for (int i = 0; i < verticies.size(); ++i)
		{
			vertexBuffer->CopyData(i, verticies[i]);
		}

		renderItem->Geo->VertexBufferGPU = vertexBuffer->Resource();
	}
}
#include "MainPassConstantBufferUpdater.h"
#include "FrameResource.h"
#include "GameTimer.h"
#include "MainPassDataProvider.h"
#include "UploadBuffer.h"
#include <DirectXMath.h>

using namespace DirectX;

MainPassConstantBufferUpdater::MainPassConstantBufferUpdater(MainPassDataProvider& mainPassDataProvider) : mainPassDataProvider { mainPassDataProvider }
{

}

void MainPassConstantBufferUpdater::update(FrameResource& frameResource, const GameTimer& gameTimer)
{
	auto mainPassConstantBuffer = frameResource.PassCB.get();
	auto mainPassData = mainPassDataProvider.getMainPassData();
	updateMainPassConstantBuffer(mainPassConstantBuffer, mainPassData, gameTimer);
}

void MainPassConstantBufferUpdater::updateMainPassConstantBuffer(UploadBuffer<PassConstants>* mainPassConstantBuffer, MainPassData mainPassData, const GameTimer& gameTimer)
{
	PassConstants passConstants = passConstantsFromMainPassData(mainPassData, gameTimer);
	mainPassConstantBuffer->CopyData(0, passConstants);

	// Reflected pass stored in index 1
	PassConstants reflectedPassCB = updateReflectedPassCB(passConstants, gameTimer);
	mainPassConstantBuffer->CopyData(1, reflectedPassCB);
}

PassConstants MainPassConstantBufferUpdater::passConstantsFromMainPassData(MainPassData mainPassData, const GameTimer& gameTimer)
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

	mainPassCB.FogColor = mainPassData.fogColor;
	mainPassCB.gFogStart = mainPassData.fogStart;
	mainPassCB.gFogRange = mainPassData.fogRange;

	return mainPassCB;
}

PassConstants MainPassConstantBufferUpdater::updateReflectedPassCB(PassConstants mainPassCB, const GameTimer& gameTimer)
{
	PassConstants reflectedPassCB = mainPassCB;

	XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	XMMATRIX R = XMMatrixReflect(mirrorPlane);

	// Reflect the lighting.
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR lightDir = XMLoadFloat3(&mainPassCB.Lights[i].Direction);
		XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&reflectedPassCB.Lights[i].Direction, reflectedLightDir);
	}

	return reflectedPassCB;
}
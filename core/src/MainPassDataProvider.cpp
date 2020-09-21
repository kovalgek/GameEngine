#include "MainPassDataProvider.h"

using namespace DirectX;

MainPassDataProvider::MainPassDataProvider()
{
	mainPassData.sunTheta = 1.25f * DirectX::XM_PI;
	mainPassData.sunPhi = DirectX::XM_PIDIV4;
}

#pragma region listeners

void MainPassDataProvider::onCameraModelUpdated(CameraModel cameraModel)
{
	CameraModel normalizedCameraModel;
	normalizedCameraModel.theta = cameraModel.theta;
	normalizedCameraModel.phi = MathHelper::Clamp(cameraModel.phi, 0.1f, MathHelper::Pi - 0.1f);
	normalizedCameraModel.radius = MathHelper::Clamp(cameraModel.radius, 5.0f, 150.0f);

	XMFLOAT3 eyePos = eyePosition(normalizedCameraModel);
	mainPassData.eyePosition = eyePos;
	XMStoreFloat4x4(&mainPassData.mView, viewMatrix(eyePos));
}

void MainPassDataProvider::onLightModelUpdated(LightModel lightModel)
{
	mainPassData.ambientLight = lightModel.ambientLight;
	mainPassData.direction = lightModel.direction;
	mainPassData.strength = lightModel.strength;
}

void MainPassDataProvider::onFogModelUpdated(FogModel fogModel)
{
	mainPassData.fogColor = fogModel.color;
	mainPassData.fogStart = fogModel.start;
	mainPassData.fogRange = fogModel.range;
}

void MainPassDataProvider::onClientSizeUpdated(ClientSizeModel clientSizeModel)
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	float aspectRatio = static_cast<float>(clientSizeModel.clientWidth) / clientSizeModel.clientHeight;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mainPassData.mProj, P);
}

#pragma endregion listeners


#pragma region shortcuts

DirectX::XMFLOAT3 MainPassDataProvider::eyePosition(CameraModel cameraModel)
{
	DirectX::XMFLOAT3 eyePosition;

	// Convert Spherical to Cartesian coordinates.
	eyePosition.x = cameraModel.radius * sinf(cameraModel.phi) * cosf(cameraModel.theta);
	eyePosition.z = cameraModel.radius * sinf(cameraModel.phi) * sinf(cameraModel.theta);
	eyePosition.y = cameraModel.radius * cosf(cameraModel.phi);

	return eyePosition;
}

XMMATRIX MainPassDataProvider::viewMatrix(DirectX::XMFLOAT3 eyePosition)
{
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return XMMatrixLookAtLH(pos, target, up);
}

#pragma endregion shortcuts
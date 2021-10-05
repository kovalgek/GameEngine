#include "MainPassDataProvider.h"
#include "Scene.h"

using namespace DirectX;

MainPassDataProvider::MainPassDataProvider(Scene& scene) : scene { scene }
{
	mainPassData.sunTheta = 1.25f * DirectX::XM_PI;
	mainPassData.sunPhi = DirectX::XM_PIDIV4;
}

MainPassDataProvider::~MainPassDataProvider() = default;

MainPassData const MainPassDataProvider::getMainPassData()
{
	updateLightData(mainPassData);
	updateCameraData(mainPassData);
	updateFogData(mainPassData);
	updateClientSize(mainPassData);

	return mainPassData;
}

void MainPassDataProvider::updateLightData(MainPassData &mainPassData)
{
	auto lightView = scene.lightView();
	auto light = lightView.front();
	auto& lightComponent = lightView.get<LightComponent>(light);

	mainPassData.ambientLight = lightComponent.ambientLight;
	mainPassData.direction = lightComponent.direction;
	mainPassData.strength = lightComponent.strength;
}

void MainPassDataProvider::updateCameraData(MainPassData& mainPassData)
{
	auto cameraView = scene.cameraView();
	auto camera = cameraView.front();
	auto& cameraComponent = cameraView.get<CameraComponent>(camera);

	CameraModel normalizedCameraModel;
	normalizedCameraModel.theta = cameraComponent.theta;
	normalizedCameraModel.phi = MathHelper::Clamp(cameraComponent.phi, 0.1f, MathHelper::Pi - 0.1f);
	normalizedCameraModel.radius = MathHelper::Clamp(cameraComponent.radius, 5.0f, 150.0f);

	XMFLOAT3 eyePos = eyePosition(normalizedCameraModel);
	mainPassData.eyePosition = eyePos;
	XMStoreFloat4x4(&mainPassData.mView, viewMatrix(eyePos));
}

void MainPassDataProvider::updateFogData(MainPassData& mainPassData)
{
	auto fogView = scene.fogView();
	auto fog = fogView.front();
	auto& fogComponent = fogView.get<FogComponent>(fog);

	mainPassData.fogColor = fogComponent.color;
	mainPassData.fogStart = fogComponent.start;
	mainPassData.fogRange = fogComponent.range;
}

void MainPassDataProvider::updateClientSize(MainPassData& mainPassData)
{
	auto view = scene.clientSizeView();
	auto clientSize = view.front();
	auto& clientSizeComponent = view.get<ClientSizeComponent>(clientSize);

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	float aspectRatio = static_cast<float>(clientSizeComponent.width) / clientSizeComponent.height;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mainPassData.mProj, P);
}

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
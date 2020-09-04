#include "MainPassDataProvider.h"

using namespace DirectX;

MainPassDataProvider::MainPassDataProvider()
{
	mainPassData.sunTheta = 1.25f * DirectX::XM_PI;
	mainPassData.sunPhi = DirectX::XM_PIDIV4;
}

void MainPassDataProvider::onResize(int clientWidth, int clientHeight)
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	float aspectRatio = static_cast<float>(clientWidth) / clientHeight;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mainPassData.mProj, P);
}

void MainPassDataProvider::updateCamera()
{
	// Convert Spherical to Cartesian coordinates.
	mainPassData.eyePosition.x = cameraModel.radius * sinf(cameraModel.phi) * cosf(cameraModel.theta);
	mainPassData.eyePosition.z = cameraModel.radius * sinf(cameraModel.phi) * sinf(cameraModel.theta);
	mainPassData.eyePosition.y = cameraModel.radius * cosf(cameraModel.phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(mainPassData.eyePosition.x, mainPassData.eyePosition.y, mainPassData.eyePosition.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mainPassData.mView, view);

	mainPassData.ambientLight = lightModel.ambientLight;
	mainPassData.direction = lightModel.direction;
	mainPassData.strength = lightModel.strength;
}

void MainPassDataProvider::onMouseDown(int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MainPassDataProvider::updateCameraModel(CameraModel cameraModel)
{
	this->cameraModel.theta = cameraModel.theta;
	this->cameraModel.phi = MathHelper::Clamp(cameraModel.phi, 0.1f, MathHelper::Pi - 0.1f);
	this->cameraModel.radius = MathHelper::Clamp(cameraModel.radius, 5.0f, 150.0f);
}

void MainPassDataProvider::updateLight(LightModel lightModel)
{
	this->lightModel = lightModel;
}

void MainPassDataProvider::onMouseMove(int btnState, int x, int y)
{
	if (btnState == 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		cameraModel.theta += dx;
		cameraModel.phi += dy;

		// Restrict the angle mPhi.
		cameraModel.phi = MathHelper::Clamp(cameraModel.phi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else 
		if (btnState == 1)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f * static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		cameraModel.radius += dx - dy;

		// Restrict the radius.
		cameraModel.radius = MathHelper::Clamp(cameraModel.radius, 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
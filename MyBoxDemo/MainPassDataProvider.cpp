#include "MainPassDataProvider.h"

using namespace DirectX;

void MainPassDataProvider::onResize(int clientWidth, int clientHeight)
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	float aspectRatio = static_cast<float>(clientWidth) / clientHeight;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, aspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void MainPassDataProvider::updateCamera()
{
	// Convert Spherical to Cartesian coordinates.
	eyePosition.x = mRadius * sinf(mPhi) * cosf(mTheta);
	eyePosition.z = mRadius * sinf(mPhi) * sinf(mTheta);
	eyePosition.y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(eyePosition.x, eyePosition.y, eyePosition.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void MainPassDataProvider::onMouseDown(int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void MainPassDataProvider::onMouseMove(int btnState, int x, int y)
{
	if (btnState == 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if (btnState == 1)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f * static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

MainPassData MainPassDataProvider::getMainPassData()
{
	return MainPassData{ mView , mProj, eyePosition };
}

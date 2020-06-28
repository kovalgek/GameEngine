#include "MathHelper.h"
#include "MainPassData.h"
#include <DirectXMath.h>

#pragma once
class MainPassDataProvider
{
public:
	void onResize(int clientWidth, int clientHeight);
	void updateCamera();
	void onMouseDown(int x, int y);
	void onMouseMove(int btnState, int x, int y);

	MainPassData getMainPassData();

private:
	DirectX::XMFLOAT3 eyePosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * DirectX::XM_PI;
	float mPhi = 0.2f * DirectX::XM_PI;
	float mRadius = 15.0f;

	POINT mLastMousePos;

	float mSunTheta = 1.25f * DirectX::XM_PI;
	float mSunPhi = DirectX::XM_PIDIV4;
};


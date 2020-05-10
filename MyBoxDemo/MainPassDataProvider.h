#include <DirectXMath.h>
#include <windows.h>
#include "MathHelper.h"
#include "MainPassData.h"

#pragma once
class MainPassDataProvider
{
public:
	void onResize(int clientWidth, int clientHeight);
	void updateCamera();
	void onMouseDown(int x, int y);
	void onMouseMove(int x, int y);

	MainPassData getMainPassData();

private:
	DirectX::XMFLOAT3 eyePosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * DirectX::XM_PI;
	float mPhi = 0.2f * DirectX::XM_PI;
	float mRadius = 15.0f;

	POINT mLastMousePos;
};


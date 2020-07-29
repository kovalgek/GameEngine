#include "MathHelper.h"
#include "MainPassData.h"
#include <DirectXMath.h>

struct CameraModel
{
	float theta = DirectX::XM_PI;
	float phi = DirectX::XM_PIDIV4;
	float radius = 70;
};

struct LightModel
{
	DirectX::XMFLOAT4 ambientLight;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 strength;
};

#pragma once
class MainPassDataProvider
{
public:
	void onResize(int clientWidth, int clientHeight);
	void updateCamera();
	void onMouseDown(int x, int y);
	void onMouseMove(int btnState, int x, int y);
	void updateData(CameraModel cameraModel);

	void updateLight(LightModel lightModel);


	MainPassData const getMainPassData() { return mainPassData; }

	MainPassDataProvider();

private:
	//float mTheta = 1.5f * DirectX::XM_PI;
	//float mPhi = 0.2f * DirectX::XM_PI;
	//float mRadius = 15.0f;

	POINT mLastMousePos;

	CameraModel cameraModel;
	LightModel lightModel;
	MainPassData mainPassData;
};


#include <DirectXMath.h>
#include "MathHelper.h"
#include "MainPassData.h"
#include "MainPassModelsListener.h"
#include "MainPassModels.h"

#pragma once
class MainPassDataProvider : public MainPassModelsListener
{
public:
	MainPassDataProvider();

	// main API
	MainPassData const getMainPassData() { return mainPassData; }

	// MainPassModelsListener API
	void onLightModelUpdated(LightModel lightModel);
	void onCameraModelUpdated(CameraModel cameraModel);
	void onClientSizeUpdated(ClientSizeModel clientSizeModel);

private:
	MainPassData mainPassData;

	DirectX::XMFLOAT3 eyePosition(CameraModel cameraModel);
	DirectX::XMMATRIX viewMatrix(DirectX::XMFLOAT3 eyePosition);
};


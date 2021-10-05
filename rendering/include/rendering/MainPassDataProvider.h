#include <DirectXMath.h>
#include "MathHelper.h"
#include "MainPassData.h"
#include "MainPassModels.h"

class Scene;

#pragma once
class MainPassDataProvider
{
public:
	MainPassDataProvider(Scene &scene);
	~MainPassDataProvider();

	// main API
	MainPassData const getMainPassData();

private:
	Scene& scene;
	MainPassData mainPassData;

	void updateLightData(MainPassData& mainPassData);
	void updateCameraData(MainPassData& mainPassData);
	void updateFogData(MainPassData& mainPassData);
	void updateClientSize(MainPassData& mainPassData);

	DirectX::XMFLOAT3 eyePosition(CameraModel cameraModel);
	DirectX::XMMATRIX viewMatrix(DirectX::XMFLOAT3 eyePosition);
};


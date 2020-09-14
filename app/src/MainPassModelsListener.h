#pragma once

struct LightModel;
struct CameraModel;
struct ClientSizeModel;

class MainPassModelsListener
{
public:
	virtual void onLightModelUpdated(LightModel lightModel) = 0;
	virtual void onCameraModelUpdated(CameraModel cameraModel) = 0;
	virtual void onClientSizeUpdated(ClientSizeModel clientSizeModel) = 0;
};

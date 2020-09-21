#pragma once

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

struct ClientSizeModel
{
	int clientWidth;
	int clientHeight;
};
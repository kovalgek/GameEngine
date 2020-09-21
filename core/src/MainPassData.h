#include "MathHelper.h"

struct MainPassLight
{
	DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };
	DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
};

#pragma once
struct MainPassData
{
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 eyePosition = { 0.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT4 ambientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	//std::vector<MainPassLight> lights;

	DirectX::XMFLOAT3 strength = { 0.5f, 0.5f, 0.5f };
	DirectX::XMFLOAT3 direction = { 0.5f, 0.5f, 0.5f };

	float sunTheta;
	float sunPhi;
	int clientWidth;
	int clientHeight;
};
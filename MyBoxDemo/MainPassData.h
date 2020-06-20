#include "MathHelper.h"

#pragma once
struct MainPassData
{
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 eyePosition = { 0.0f, 0.0f, 0.0f };
	float sunTheta;
	float sunPhi;
	int clientWidth;
	int clientHeight;
};
#pragma once
#define _USE_MATH_DEFINES

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "MathHelper.h"
#include <d3d12.h>
#include <math.h>
#include <entt.hpp>

struct Material;
struct MeshGeometry;

enum class ComponentType : int {
	RenderComponentType = 0,
	Mirror,
};

struct Component {
	ComponentType componentType = ComponentType::RenderComponentType;
	virtual ~Component() = default;
};

enum class RenderLayer : int
{
	Opaque = 0,
	Mirrors,
	Reflected,
	Transparent,
	Shadow,
	AlphaTested,
	Count
};

struct RenderComponent : public Component
{
	RenderComponent() {
		componentType = ComponentType::RenderComponentType;
	};
	~RenderComponent() = default;

	DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 texture = MathHelper::Identity4x4();

	// For frame
	int numberOfDirtyFrames = 3;
	UINT objectConstantBufferIndex = -1;

	Material* material = nullptr;
	MeshGeometry* geometry = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;

	std::set<RenderLayer> layers;
};

struct MirrorComponent : public Component {
	MirrorComponent() {
		componentType = ComponentType::Mirror;
	};
	~MirrorComponent() = default;

	DirectX::XMVECTOR mirrorPlane = DirectX::XMVectorZero();// XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
};

struct ShadowComponent
{
	Material* material = nullptr;
	MeshGeometry* geometry = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
};

struct ReflectionComponent
{
	Material* material = nullptr;
	MeshGeometry* geometry = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
};

struct CameraComponent
{
	float theta = M_PI;
	float phi = M_PI_4;
	float radius = 70;
};

struct LightComponent
{
	DirectX::XMFLOAT4 ambientLight;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 strength;
};

struct FogComponent
{
	DirectX::XMFLOAT4 color;
	float start;
	float range;
};

struct ClientSizeComponent
{
	int width;
	int height;
};

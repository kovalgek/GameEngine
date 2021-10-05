#pragma once
#define _USE_MATH_DEFINES

#include <vector>
#include <string>
#include <memory>
#include <entt.hpp>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "MathHelper.h"
#include <d3d12.h>
#include <math.h>

class GeometryStorage;
class MaterialsDataProvider;
class GameTimer;

struct Material;
struct MeshGeometry;

struct TransformComponent
{
	DirectX::XMFLOAT4X4 world = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 texture = MathHelper::Identity4x4();
};

struct RenderComponent
{
	RenderComponent() = default;
	~RenderComponent() = default;

	Material* material = nullptr;
	MeshGeometry* geometry = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
	
	// For frame
	int numberOfDirtyFrames = 3;
	UINT objectConstantBufferIndex = -1;
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

enum class RenderLayer2 : int
{
	Opaque = 0,
	Mirrors,
	Reflected,
	Transparent,
	Shadow,
	AlphaTested,
	Count
};

using RenderAndTransformGroup = entt::basic_group<enum entt::entity, entt::exclude_t<>, entt::get_t<RenderComponent>, TransformComponent>;
using RenderView = entt::basic_view<enum entt::entity, entt::exclude_t<>, RenderComponent>;
using LightEntityView = entt::basic_view<enum entt::entity, entt::exclude_t<>, LightComponent>;
using CameraEntityView = entt::basic_view<enum entt::entity, entt::exclude_t<>, CameraComponent>;
using FogEntityView = entt::basic_view<enum entt::entity, entt::exclude_t<>, FogComponent>;
using ClientSizeView = entt::basic_view<enum entt::entity, entt::exclude_t<>, ClientSizeComponent>;

class Scene
{
public:
	Scene(
		const GeometryStorage& geometryStorage,
		const MaterialsDataProvider& materialsDataProvider
	);
	~Scene();

	RenderAndTransformGroup renderAndTransformGroup();
	RenderView renderView();
	LightEntityView lightView();
	CameraEntityView cameraView();
	FogEntityView fogView();
	ClientSizeView clientSizeView();

	// system events
	void update(const GameTimer& gameTimer);
	void onWindowResize(int clientWidth, int clientHeight);
	void onMouseDown(int x, int y);
	void onMouseMove(int btnState, int x, int y);

	entt::entity Scene::createRenderItem(
		std::string meshName,
		std::string submeshName,
		std::string material,
		std::vector<float> position,
		std::vector<float> scaling,
		std::vector<float> textureTransform
	);

private:
	entt::registry registry;

	void createLight();
	void createCamera();
	void createFog();
	void createClientSize();

	void addShadow(entt::entity entity);
	void addReflection(entt::entity entity);

	const GeometryStorage& geometryStorage;
	const MaterialsDataProvider& materialsDataProvider;

	void sceneDidLoad();

	int itemIndex = 0;	
};
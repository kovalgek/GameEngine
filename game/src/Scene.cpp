#include "Scene.h"
#include "MeshGeometry.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"
#include <algorithm>
#include "GameTimer.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

Scene::Scene(
	const GeometryStorage& geometryStorage,
	const MaterialsDataProvider& materialsDataProvider
) : 
	geometryStorage{ geometryStorage },
	materialsDataProvider{ materialsDataProvider }
{
	sceneDidLoad();
}

Scene::~Scene() = default;


void Scene::sceneDidLoad()
{
	createCamera();
	createLight();
	createFog();
	createClientSize();

	entt::entity entity = createRenderItem(
		"shapeGeo",
		"box",
		"water",
		{ 0.0f,4.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 5.0f,5.0f,5.0f }
	);

	entt::entity entity2 = createRenderItem(
		"shapeGeo",
		"box",
		"grass",
		{ 0.0f,4.0f,4.0f },
		{ 1.0f,1.0f,1.0f },
		{ 5.0f,5.0f,5.0f }
	);
}

void Scene::addShadow(entt::entity entity)
{
	RenderComponent &renderComponent = registry.get<RenderComponent>(entity);

	registry.emplace<ShadowComponent>(
		entity,
		renderComponent.material,
		renderComponent.geometry,
		renderComponent.primitiveType,
		renderComponent.indexCount,
		renderComponent.startIndexLocation,
		renderComponent.baseVertexLocation
	);
}

void Scene::addReflection(entt::entity entity)
{

}

RenderView Scene::renderView()
{
	return registry.view<RenderComponent>();
}

RenderAndTransformGroup Scene::renderAndTransformGroup()
{
	return registry.group<TransformComponent>(entt::get<RenderComponent>);
}

LightEntityView Scene::lightView()
{
	return registry.view<LightComponent>();
}

CameraEntityView Scene::cameraView()
{
	return registry.view<CameraComponent>();
}

FogEntityView Scene::fogView()
{
	return registry.view<FogComponent>();
}

ClientSizeView Scene::clientSizeView()
{
	return registry.view<ClientSizeComponent>();
}

entt::entity Scene::createRenderItem(
	std::string meshName,
	std::string submeshName,
	std::string material,
	std::vector<float> position,
	std::vector<float> scaling,
	std::vector<float> textureTransform
)
{
	entt::entity entity = registry.create();

	XMFLOAT4X4 word;
	XMStoreFloat4x4(&word, XMMatrixScaling(scaling[0], scaling[1], scaling[2]) * XMMatrixTranslation(position[0], position[1], position[2]));
	registry.emplace<TransformComponent>(entity, word);

	auto geometry = geometryStorage.getGeometry(meshName);
	registry.emplace<RenderComponent>(
		entity,
		materialsDataProvider.getMaterialForName(material),
		geometry,
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		geometry->DrawArgs[submeshName].IndexCount,
		geometry->DrawArgs[submeshName].StartIndexLocation,
		geometry->DrawArgs[submeshName].BaseVertexLocation,
		3,
		(UINT)itemIndex++
	);
	return entity;
}

void Scene::createLight()
{
	entt::entity light = registry.create();

	DirectX::XMFLOAT4 ambientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 strenght = { 0.0f, 0.0f, 0.0f };

	registry.emplace<LightComponent>(light, ambientLight, direction, strenght);
}

void Scene::createCamera()
{
	entt::entity cameraEntity = registry.create();
	registry.emplace<CameraComponent>(cameraEntity, (float)M_PI, (float)M_PI_4, 70.0f);
}

void Scene::createFog()
{
	entt::entity fogEntity = registry.create();

	DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
	float start = 10;
	float range = 100;

	registry.emplace<FogComponent>(fogEntity, color, start, range);
}

void Scene::createClientSize()
{
	entt::entity clientSize = registry.create();
	registry.emplace<ClientSizeComponent>(clientSize, 0, 0);
}

void Scene::update(const GameTimer& gameTimer)
{

}

void Scene::onWindowResize(int clientWidth, int clientHeight)
{
	auto view = clientSizeView();
	auto clientSize = view.front();
	auto& clientSizeComponent = view.get<ClientSizeComponent>(clientSize);

	clientSizeComponent.width = clientWidth;
	clientSizeComponent.height = clientHeight;
}

void Scene::onMouseDown(int x, int y)
{

}

void Scene::onMouseMove(int btnState, int x, int y)
{

}


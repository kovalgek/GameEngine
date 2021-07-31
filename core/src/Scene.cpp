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

void Scene::update(const GameTimer& gameTimer)
{

}

void Scene::onWindowResize(int clientWidth, int clientHeight)
{

}

void Scene::onMouseDown(int x, int y)
{

}

void Scene::onMouseMove(int btnState, int x, int y)
{

}

void Scene::createCamera()
{
	//cameraEntity = registry.create();
	//registry.emplace<CameraComponent>(cameraEntity, M_PI, M_PI_4, 70);
}
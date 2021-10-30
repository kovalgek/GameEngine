#include "Scene.h"
#include "MeshGeometry.h"
#include "GeometryStorage.h"
#include "MaterialsDataProvider.h"
#include <algorithm>
#include "GameTimer.h"
#include "GameObject.h"

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

	//createGameObjectForRender(
	//	"shapeGeo",
	//	"box",
	//	"water",
	//	{ 0.0f,4.0f,0.0f },
	//	{ 10.0f,1.0f,10.0f },
	//	{ 5.0f,5.0f,5.0f },
	//	{ RenderLayer::Transparent }
	//);


	mirrorShadowDemo();
	//entt::entity entity2 = createRenderItem(
	//	"shapeGeo",
	//	"hills",
	//	"grass",
	//	{ 0.0f,4.0f,4.0f },
	//	{ 1.0f,1.0f,1.0f },
	//	{ 10.0f,10.0f,10.0f },
	//	{ RenderLayer::AlphaTested }
	//);
}

void Scene::mirrorShadowDemo()
{
	auto floor = createGameObjectForRender(
		"roomGeo",
		"floor",
		"checkertile",
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		{ RenderLayer::Opaque }
	);
	gameObjects.push_back(std::move(floor));

	auto wall = createGameObjectForRender(
		"roomGeo",
		"wall",
		"bricks",
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		{ RenderLayer::Opaque }
	);
	gameObjects.push_back(std::move(wall));

	auto skull = createGameObjectForRender(
		"models",
		"skull",
		"skullMat",
		{ 1.0f,1.0f,-5.0f },
		{ 0.5f,0.5f,0.5f },
		{ 1.0f,1.0f,1.0f },
		{ RenderLayer::Opaque }
	);
	gameObjects.push_back(std::move(skull));

	createMirror();
}

void Scene::createMirror() {
	auto mirror = createGameObjectForRender(
		"roomGeo",
		"mirror",
		"icemirror",
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,1.0f,1.0f },
		{ 1.0f,1.0f,1.0f },
		{ RenderLayer::Mirrors, RenderLayer::Transparent }
	);

	auto mirrorComponent = std::make_unique<MirrorComponent>();
	mirrorComponent->mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	mirror->components.push_back(std::move(mirrorComponent));

	gameObjects.push_back(std::move(mirror));
}

std::unique_ptr<GameObject> Scene::createGameObjectForRender(
	std::string meshName,
	std::string submeshName,
	std::string material,
	std::vector<float> position,
	std::vector<float> scaling,
	std::vector<float> textureTransform,
	std::set<RenderLayer> layers
)
{
	auto gameObject = std::make_unique<GameObject>();

	auto renderComponent = std::make_unique<RenderComponent>();
	XMStoreFloat4x4(&renderComponent->world, XMMatrixScaling(scaling[0], scaling[1], scaling[2]) * XMMatrixTranslation(position[0], position[1], position[2]));
	XMStoreFloat4x4(&renderComponent->texture, XMMatrixScaling(textureTransform[0], textureTransform[1], textureTransform[2]));
	renderComponent->numberOfDirtyFrames = 3;
	renderComponent->objectConstantBufferIndex = (UINT)itemIndex++;

	auto geometry = geometryStorage.getGeometry(meshName);
	auto submeshGeometry = geometry->DrawArgs[submeshName];

	renderComponent->material = materialsDataProvider.getMaterialForName(material);
	renderComponent->geometry = geometry;
	renderComponent->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	renderComponent->indexCount = submeshGeometry.IndexCount;
	renderComponent->startIndexLocation = submeshGeometry.StartIndexLocation;
	renderComponent->baseVertexLocation = submeshGeometry.BaseVertexLocation;
	renderComponent->layers = layers;
	
	gameObject->components.push_back(std::move(renderComponent));

	return gameObject;
}

GameObject &Scene::addGameObject(std::unique_ptr<GameObject> gameObject)
{
	auto &go = gameObjects.emplace_back(std::move(gameObject));
	return *go.get();
}

std::vector<RenderComponent*> Scene::renderComponents()
{
	std::vector<RenderComponent*> result;
	for (auto& gameObject : gameObjects) {
		for (auto component : gameObject->allComponents()) {
			if (component->componentType == ComponentType::RenderComponentType) {
				RenderComponent *renderComponent = dynamic_cast<RenderComponent*>(component);
				result.push_back(renderComponent);
			}
		}
	}
	return result;
}

std::vector<RenderComponent*> Scene::renderComponents(RenderLayer renderLayer)
{
	std::vector<RenderComponent*> result;
	for (auto& gameObject : gameObjects) {
		for (auto& component : gameObject->allComponents()) {
			if (component->componentType == ComponentType::RenderComponentType) {
				auto renderComponent = dynamic_cast<RenderComponent*>(component);
				if (renderComponent->layers.find(renderLayer) != renderComponent->layers.end()) {
					result.push_back(renderComponent);
				}
			}
		}
	}
	return result;
}

MirrorComponent* Scene::mirrorComponent()
{
	for (auto& gameObject : gameObjects) {
		for (auto component : gameObject->allComponents()) {
			if (component->componentType == ComponentType::Mirror) {
				return dynamic_cast<MirrorComponent*>(component);
			}
		}
	}
	return nullptr;
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
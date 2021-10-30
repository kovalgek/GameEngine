#pragma once
#define _USE_MATH_DEFINES

#include <vector>
#include <string>
#include <memory>
#include "Components.h"

class GeometryStorage;
class MaterialsDataProvider;
class GameTimer;
class GameObject;

//using RenderAndTransformGroup = entt::basic_group<enum entt::entity, entt::exclude_t<>, entt::get_t<RenderComponent>, TransformComponent>;
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

	//RenderAndTransformGroup renderAndTransformGroup();
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


	std::unique_ptr<GameObject> createGameObjectForRender(
		std::string meshName,
		std::string submeshName,
		std::string material,
		std::vector<float> position,
		std::vector<float> scaling,
		std::vector<float> textureTransform,
		std::set<RenderLayer>
	);

	GameObject &addGameObject(std::unique_ptr<GameObject> gameObject);

	std::vector<RenderComponent*> renderComponents(RenderLayer renderLayer);
	std::vector<RenderComponent*> renderComponents();

	MirrorComponent *mirrorComponent();
	

private:
	entt::registry registry;

	void createLight();
	void createCamera();
	void createFog();
	void createClientSize();

	void createMirror();

	//void addShadow(entt::entity entity);
	//void addReflection(entt::entity entity);

	const GeometryStorage& geometryStorage;
	const MaterialsDataProvider& materialsDataProvider;

	void sceneDidLoad();

	int itemIndex = 0;	

	void mirrorShadowDemo();

	std::vector<std::unique_ptr<GameObject>> gameObjects;
};
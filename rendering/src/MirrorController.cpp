#include "MirrorController.h"
#include "Scene.h"
#include "GameObject.h"

using namespace DirectX;

MirrorController::MirrorController(Scene& scene) : scene { scene }
{
	createReflections();
}

void MirrorController::createReflections()
{
	// create all objects that are reflected
	auto reflectedSkullGO = scene.createGameObjectForRender(
		"models",
		"skull",
		"skullMat",
		{ 1.0f,1.0f,-5.0f },
		{ 0.5f,0.5f,0.5f },
		{ 1.0f,1.0f,1.0f },
		{ RenderLayer::Reflected }
	);

	scene.addGameObject(std::move(reflectedSkullGO));
}

// update their position simmetric to mirror
void MirrorController::updateReflections()
{
	auto mirror = scene.mirrorComponent();

	//// Update reflection world matrix.
	//XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	//XMMATRIX R = XMMatrixReflect(mirrorPlane);
	//XMStoreFloat4x4(&mReflectedSkullRitem->World, skullWorld * R);
}
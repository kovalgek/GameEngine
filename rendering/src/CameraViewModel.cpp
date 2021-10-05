#include "CameraViewModel.h"
#include "Scene.h"

CameraViewModel::CameraViewModel(Scene& scene) : scene{ scene }
{

}

void CameraViewModel::update()
{
	auto cameraView = scene.cameraView();
	auto camera = cameraView.front();
	auto& cameraComponent = cameraView.get<CameraComponent>(camera);

	cameraComponent.theta = theta;
	cameraComponent.phi = phi;
	cameraComponent.radius = radius;
}


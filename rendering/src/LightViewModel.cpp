#include "LightViewModel.h"
#include "Scene.h"

LightViewModel::LightViewModel(Scene& scene) : scene{ scene }
{
	update();
}

void LightViewModel::update()
{
	auto lightView = scene.lightView();
	auto light = lightView.front();
	auto& lightComponent = lightView.get<LightComponent>(light);
	lightComponent.ambientLight = DirectX::XMFLOAT4(ambient[0], ambient[1], ambient[2], ambient[3]);
	lightComponent.direction = DirectX::XMFLOAT3(direction[0], direction[1], direction[2]);
}
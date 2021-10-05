#include "FogViewModel.h"
#include "Scene.h"

FogViewModel::FogViewModel(Scene& scene) : scene { scene }
{

}

void FogViewModel::update()
{
	auto fogView = scene.fogView();
	auto fog = fogView.front();
	auto& fogComponent = fogView.get<FogComponent>(fog);

	fogComponent.color = DirectX::XMFLOAT4(color.x, color.y, color.z, color.w);
	fogComponent.start = start;
	fogComponent.range = range;
}
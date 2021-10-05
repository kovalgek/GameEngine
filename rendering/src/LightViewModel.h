#pragma once

#include "ViewModel.h"

class Scene;

class LightViewModel : public ViewModel
{

public:
	LightViewModel(Scene& scene);
	~LightViewModel() = default;

	void update();

	float ambient[4] = { 1,1,1,0 };
	float direction[3] = { 0,0,0 };

private:
	Scene& scene;
};
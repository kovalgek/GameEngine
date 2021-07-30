#pragma once

#include "View.h"

class LightViewModel;

class LightView : public View
{
public:
	LightView(LightViewModel& viewModel);
	~LightView() = default;

	void present();

private:

	LightViewModel& viewModel;

	float ambient[4];
	float direction[3];
};
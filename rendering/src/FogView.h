#pragma once

#include "View.h"
#include "imgui.h"

class FogViewModel;

class FogView : public View
{
public:
	FogView(FogViewModel& viewModel);
	~FogView() = default;

	ViewModel& getViewModel();
	void present();

private:

	FogViewModel& viewModel;

	ImVec4 color;
	float start;
	float range;
};


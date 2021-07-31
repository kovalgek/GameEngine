#pragma once

#include "ViewModel.h"
#include "imgui.h"

class FogViewModel : public ViewModel
{
public:
	FogViewModel() = default;
	~FogViewModel() = default;

	void update();

	ImVec4 color = ImVec4(0, 0, 0, 0);
	float start = 40;
	float range = 100;
};
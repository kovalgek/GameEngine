#pragma once

#include "imgui.h"

class FogViewModel {

public:
	FogViewModel() = default;
	~FogViewModel() = default;

	ImVec4 color = ImVec4(0, 0, 0, 0);
	float start = 40;
	float range = 100;
};
#pragma once

class LightViewModel {

public:
	LightViewModel() = default;
	~LightViewModel() = default;


	float ambient[4] = { 1,1,1,0 };
	float direction[3] = { 0,0,0 };
};
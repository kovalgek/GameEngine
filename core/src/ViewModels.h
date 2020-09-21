#define _USE_MATH_DEFINES
#include <math.h>

#pragma once
struct CameraViewModel
{
	float theta = M_PI;
	float phi = M_PI_4;
	float radius = 70;
};

struct PrimitiveViewModel
{
	std::string currentMesh;
	std::string currentSubMesh;
	std::string currentMaterial;
	float position[3];
	float scaling[3];
	float texture[3];
};

struct LightsViewModel
{
	float ambient[4];
	float direction[3];
	float strength[3];
};
#pragma once

#include "View.h"
#include <string>

class PrimitiveViewModel;

class PrimitiveView : public View
{
public:
	PrimitiveView(PrimitiveViewModel& viewModel);
	~PrimitiveView() = default;

	void present();

private:

	void createMeshCombo();
	void createSubmeshCombo();
	void createMaterialCombo();

	PrimitiveViewModel& viewModel;

	std::string mesh;
	std::string subMesh;
	std::string material;
	float position[3];
	float scaling[3];
	float texture[3];
};
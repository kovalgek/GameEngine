#pragma once

#include "View.h"

class CameraViewModel;

class CameraView : public View
{
public:
	CameraView(CameraViewModel& viewModel);
	~CameraView() = default;

	void present();

private:

	CameraViewModel& viewModel;

	float theta;
	float phi;
	float radius;
};


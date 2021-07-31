#pragma once

#include "View.h"

class CameraViewModel;

class CameraView : public View
{
public:
	CameraView(CameraViewModel& viewModel);
	~CameraView() = default;

	ViewModel& getViewModel();
	void present();

private:

	CameraViewModel& viewModel;

	float theta;
	float phi;
	float radius;
};


#include "CameraView.h"
#include "imgui.h"
#include <DirectXMath.h>
#include "CameraViewModel.h"

using namespace DirectX;
using namespace ImGui;

CameraView::CameraView(CameraViewModel& viewModel) : viewModel {viewModel}
{
	theta = viewModel.theta;
	phi = viewModel.phi;
	radius = viewModel.radius;
}

void CameraView::present()
{
	Begin("Camera");
	if (SliderFloat("theta", &theta, 0.0f, XM_2PI)) {
		viewModel.theta = theta;
	}
	if (SliderFloat("phi", &phi, 0.0f, XM_PI)) {
		viewModel.phi = phi;
	}
	if (SliderFloat("radius", &radius, 0.0f, 150.0f)) {
		viewModel.radius = radius;
	}
	End();
}
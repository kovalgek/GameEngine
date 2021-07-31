#include "FogView.h"
#include "FogViewModel.h"


using namespace ImGui;

FogView::FogView(FogViewModel& viewModel) : viewModel{ viewModel }
{
	color = viewModel.color;
	start = viewModel.start;
	range = viewModel.range;
}

void FogView::present()
{
	Begin("Fog");
	if (ColorPicker4("##picker", (float*)&color, ImGuiColorEditFlags_NoSmallPreview))
	{
		viewModel.color = color;

	}
	if (SliderFloat("start", &start, 0.0f, 100.0f))
	{
		viewModel.start = start;

	}
	if (SliderFloat("range", &range, 0.0f, 100.0f))
	{
		viewModel.range = range;

	}
	End();
}

ViewModel& FogView::getViewModel()
{
	return viewModel;
}

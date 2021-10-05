#include "LightView.h"
#include "imgui.h"
#include "LightViewModel.h"
#include <algorithm>
#include <iterator>

using namespace ImGui;

LightView::LightView(LightViewModel& viewModel) : viewModel{ viewModel }
{
	std::copy(std::begin(viewModel.ambient), std::end(viewModel.ambient), std::begin(ambient));
	std::copy(std::begin(viewModel.direction), std::end(viewModel.direction), std::begin(direction));
}

void LightView::present()
{
	Begin("Light");
	if (InputFloat4("ambient", ambient))
	{		
		std::copy(std::begin(ambient), std::end(ambient), std::begin(viewModel.ambient));
	}

	if (InputFloat3("direction", direction))
	{
		std::copy(std::begin(direction), std::end(direction), std::begin(viewModel.direction));
	}
	End();
}

ViewModel& LightView::getViewModel()
{
	return viewModel;
}
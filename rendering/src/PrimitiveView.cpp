#include "PrimitiveView.h"
#include "PrimitiveViewModel.h"
#include "imgui.h"

using namespace ImGui;

PrimitiveView::PrimitiveView(PrimitiveViewModel& viewModel) : viewModel{ viewModel }
{
	std::copy(std::begin(viewModel.position), std::end(viewModel.position), std::begin(position));
	std::copy(std::begin(viewModel.scaling), std::end(viewModel.scaling), std::begin(scaling));
	std::copy(std::begin(viewModel.texture), std::end(viewModel.texture), std::begin(texture));
	viewModel.mesh = mesh;
	viewModel.subMesh = subMesh;
	viewModel.material = material;
}


void PrimitiveView::present()
{
	ImGui::Begin("Factory");

	createMeshCombo();
	createSubmeshCombo();
	createMaterialCombo();

	if (ImGui::InputFloat3("position", position))
	{
		std::copy(std::begin(position), std::end(position), std::begin(viewModel.position));
	}
	if (ImGui::InputFloat3("scaling", scaling))
	{
		std::copy(std::begin(scaling), std::end(scaling), std::begin(viewModel.scaling));
	}
	if (ImGui::InputFloat3("texture", texture))
	{
		std::copy(std::begin(texture), std::end(texture), std::begin(viewModel.texture));
	}

	if (ImGui::Button("Create primitive"))
	{
		viewModel.createPrimitive();
	}

	ImGui::End();
}


void PrimitiveView::createMeshCombo()
{
	if (ImGui::BeginCombo("mesh", mesh.c_str()))
	{
		for (auto iMesh : viewModel.meshes)
		{
			bool is_selected = mesh == iMesh.first;
			if (ImGui::Selectable(iMesh.first.c_str(), is_selected))
			{
				mesh = iMesh.first;
				subMesh = iMesh.second[0];

				viewModel.mesh = mesh;
				viewModel.subMesh = subMesh;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void PrimitiveView::createSubmeshCombo()
{
	if (ImGui::BeginCombo("submesh", subMesh.c_str()))
	{
		for (auto iSubmesh : viewModel.meshes[mesh])
		{
			bool is_selected = subMesh == iSubmesh;
			if (ImGui::Selectable(iSubmesh.c_str(), is_selected))
			{
				subMesh = iSubmesh;
				viewModel.subMesh = subMesh;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void PrimitiveView::createMaterialCombo()
{
	if (ImGui::BeginCombo("materials", material.c_str()))
	{
		for (auto iMaterial : viewModel.materials)
		{
			bool is_selected = material == iMaterial;
			if (ImGui::Selectable(iMaterial.c_str(), is_selected))
			{
				material = iMaterial;
				viewModel.material = material;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

ViewModel& PrimitiveView::getViewModel()
{
	return viewModel;
}
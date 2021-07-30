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
		//auto model = primitiveModel(primitiveViewModel);
		//objectsDataProvider.createPrimitive(model);
	}

	ImGui::End();
}


void PrimitiveView::createMeshCombo()
{
	if (ImGui::BeginCombo("mesh", mesh.c_str()))
	{
		for (auto mesh : viewModel.meshes)
		{
			bool is_selected = this->mesh == mesh.first;
			if (ImGui::Selectable(mesh.first.c_str(), is_selected))
			{
				this->mesh = mesh.first;
				this->subMesh = mesh.second[0];
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
	if (ImGui::BeginCombo("submesh", this->subMesh.c_str()))
	{
		for (auto submesh : viewModel.meshes[this->mesh])
		{
			bool is_selected = this->subMesh == submesh;
			if (ImGui::Selectable(submesh.c_str(), is_selected))
			{
				this->subMesh = submesh;
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
		for (auto material : viewModel.materials)
		{
			bool is_selected = this->material == material;
			if (ImGui::Selectable(material.c_str(), is_selected))
			{
				this->material = material;
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}
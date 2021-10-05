#include "MaterialConstantBufferUpdater.h"
#include "MaterialsDataProvider.h"
#include "FrameResource.h"
#include "GameTimer.h"
#include "UploadBuffer.h"
#include "Material.h"
#include <DirectXMath.h>

using namespace DirectX;

MaterialConstantBufferUpdater::MaterialConstantBufferUpdater(const MaterialsDataProvider& materialsDataProvider) : materialsDataProvider{ materialsDataProvider }
{

}

void MaterialConstantBufferUpdater::update(FrameResource& frameResource, const GameTimer& gameTimer)
{
	auto materialConstantBuffer = frameResource.MaterialCB.get();
	auto materials = materialsDataProvider.getMaterials();
	updateMaterialConstantBuffer(materialConstantBuffer, materials);
}

void MaterialConstantBufferUpdater::updateMaterialConstantBuffer(UploadBuffer<MaterialConstants>* materialConstantBuffer, std::vector<Material*> materials)
{
	for (Material* material : materials)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		if (material->NumFramesDirty == 0)
		{
			continue;
		}
		// Next FrameResource need to be updated too.
		material->NumFramesDirty--;

		MaterialConstants matConstants = materialConstantsFromMaterial(material);
		materialConstantBuffer->CopyData(material->MatCBIndex, matConstants);
	}
}

MaterialConstants MaterialConstantBufferUpdater::materialConstantsFromMaterial(Material* material)
{
	MaterialConstants matConstants;

	XMMATRIX matTransform = XMLoadFloat4x4(&material->MatTransform);
	matConstants.DiffuseAlbedo = material->DiffuseAlbedo;
	matConstants.FresnelR0 = material->FresnelR0;
	matConstants.Roughness = material->Roughness;
	XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

	return matConstants;
}
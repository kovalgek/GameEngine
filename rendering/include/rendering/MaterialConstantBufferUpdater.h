#pragma once

#include "ConstantBufferUpdating.h"
#include <vector>

template<typename T> class UploadBuffer;
class MaterialsDataProvider;
struct MaterialConstants;
struct Material;

class MaterialConstantBufferUpdater : public ConstantBufferUpdating
{
public:
	MaterialConstantBufferUpdater(const MaterialsDataProvider& materialsDataProvider);
	~MaterialConstantBufferUpdater() = default;

	void update(FrameResource & frameResource, const GameTimer & gameTimer);

private:
	const MaterialsDataProvider& materialsDataProvider;

	void updateMaterialConstantBuffer(UploadBuffer<MaterialConstants>* materialConstantBuffer, std::vector<Material*> materials);
	MaterialConstants materialConstantsFromMaterial(Material* material);
};
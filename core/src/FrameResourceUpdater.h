#include "MainPassData.h"
#include <memory>
#include <vector>

struct RenderItem;
class FrameResourceController;
class MainPassDataProvider;
class ObjectsDataProvider;
class MaterialsDataProvider;
class GameTimer;
struct ID3D12Fence;
class Waves;
struct Material;
struct FrameResource;

template<typename T> class UploadBuffer;
struct ObjectConstants;
struct MaterialConstants;
struct PassConstants;
struct Vertex;

#pragma once
class FrameResourceUpdater
{
	
public:
	FrameResourceUpdater(
		std::unique_ptr<FrameResourceController> frameResourceController,
		ID3D12Fence* fence,
		MainPassDataProvider* mainPassDataProvider,
		ObjectsDataProvider* objectsDataProvider,
		MaterialsDataProvider *materialsDataProvider,
		Waves* waves
	);
	~FrameResourceUpdater();

	void update(const GameTimer& gameTimer);

private:
	std::unique_ptr<FrameResourceController> frameResourceController;
	ID3D12Fence* fence;
	MainPassDataProvider *mainPassDataProvider;
	ObjectsDataProvider *objectsDataProvider;
	MaterialsDataProvider* materialsDataProvider;
	Waves* waves;

	void waitForFrameResourceAvailable(FrameResource* frameResource);
	
	void updateObjectConstantBufferForFrameResource(FrameResource *frameResource);
	void updateObjectConstantBuffer(UploadBuffer<ObjectConstants> *objectConstantBuffer, std::vector<RenderItem*> renderItems);
	ObjectConstants objectConstantsFromRenderItem(RenderItem* renderItem);
	
	void updateMaterialConstantBufferForFrameResource(FrameResource* frameResource);
	void updateMaterialConstantBuffer(UploadBuffer<MaterialConstants>* materialConstantBuffer, std::vector<Material*> materials);
	MaterialConstants materialConstantsFromMaterial(Material* material);

	void updateMainPassConstantBufferForFrameResource(FrameResource* frameResource, const GameTimer& gameTimer);
	void updateMainPassConstantBuffer(UploadBuffer<PassConstants>* mainPassConstantBuffer, MainPassData mainPassData, const GameTimer& gameTimer);
	PassConstants passConstantsFromMainPassData(MainPassData mainPassData, const GameTimer& gameTimer);

	void updateVertexUploadBufferForFrameResource(FrameResource* frameResource);
	void updateVertexUploadBuffer(UploadBuffer<Vertex>* vertexBuffer, std::vector<RenderItem*> renderItems);
};


#include "MainPassData.h"
#include <memory>
#include <vector>
#include <unordered_map>

struct RenderItem;
class FrameResourceController;
class MainPassDataProvider;
class ObjectsDataProvider;
class MaterialsDataProvider;
class DynamicVerticesProvider;
class DynamicVertices;
class GameTimer;
class GPUService;
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
		GPUService& gpuService,
		MainPassDataProvider& mainPassDataProvider,
		ObjectsDataProvider& objectsDataProvider,
		const MaterialsDataProvider& materialsDataProvider,
		DynamicVerticesProvider& dynamicVerticesProvider
	);
	~FrameResourceUpdater();

	void update(const GameTimer& gameTimer);

private:
	std::unique_ptr<FrameResourceController> frameResourceController;
	GPUService& gpuService;
	MainPassDataProvider& mainPassDataProvider;
	ObjectsDataProvider& objectsDataProvider;
	const MaterialsDataProvider& materialsDataProvider;
	DynamicVerticesProvider& dynamicVerticesProvider;

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
	PassConstants updateReflectedPassCB(PassConstants mainPassCB, const GameTimer& gt);
};


#pragma once

#include "ConstantBufferUpdating.h"
#include "Scene.h"

class Scene;
struct FrameResource;
class GameTimer;
template<typename T> class UploadBuffer;
struct ObjectConstants;
struct TransformComponent;

class ObjectConstantBufferUpdater : public ConstantBufferUpdating
{
public:
	ObjectConstantBufferUpdater(Scene& scene);
	~ObjectConstantBufferUpdater() = default;

	void update(FrameResource& frameResource, const GameTimer& gameTimer);

private:
	Scene& scene;

	void updateObjectConstantBuffer(UploadBuffer<ObjectConstants>* objectConstantBuffer, std::vector<RenderComponent*> renderComponents);
	ObjectConstants objectConstantsFromRenderItem(RenderComponent* renderComponent);
};
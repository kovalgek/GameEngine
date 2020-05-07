#include "d3dUtil.h"
#include <memory>
#include "RenderItem.h"


class GeometryStorage
{
public:
	GeometryStorage(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		ID3D12CommandAllocator* commandAllocator,
		ID3D12CommandQueue* commandQueue);
	~GeometryStorage();
	std::vector<std::unique_ptr<RenderItem>> renderItems();
private:
	void buildShapeGeometry();
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;

	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandQueue* commandQueue;
};


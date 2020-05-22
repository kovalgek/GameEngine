#include <memory>
#include <string>
#include <unordered_map>

struct MeshGeometry;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
class Application;

#pragma once
class GeometryStorage
{
public:
	GeometryStorage(
		Application *application,
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		ID3D12CommandAllocator* commandAllocator,
		ID3D12CommandQueue* commandQueue);
	~GeometryStorage();

	MeshGeometry *getGeometry(std::string name);
private:
	void buildShapeGeometry();
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;

	Application *application;
	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandQueue* commandQueue;
};


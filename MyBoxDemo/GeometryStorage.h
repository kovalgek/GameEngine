#include <memory>
#include <string>
#include <unordered_map>
#include <DirectXMath.h>

struct MeshGeometry;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
class Application;
class Waves;

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
	Waves* getWaves() { return waves.get(); }
private:
	void buildShapeGeometry();
	void buildLandGeometry();
	void buildWavesGeometryBuffers();

	float getHillsHeight(float x, float z)const;
	DirectX::XMFLOAT3 getHillsNormal(float x, float z)const;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;

	Application *application;
	ID3D12Device* device;
	ID3D12GraphicsCommandList* commandList;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandQueue* commandQueue;
	std::unique_ptr<Waves> waves;
};


#include <memory>
#include <string>
#include <unordered_map>
#include <windows.h>
#include <DirectXMath.h>
#include "GeometryGenerator.h"

struct MeshGeometry;
struct SubmeshGeometry;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
class Waves;

#pragma once
class GeometryStorage
{
public:
	GeometryStorage(
		ID3D12Device *const device,
		ID3D12GraphicsCommandList * const commandList
	);
	~GeometryStorage();

	MeshGeometry *getGeometry(const std::string name);
	Waves* getWaves() { return waves.get(); }

private:
	ID3D12Device *const device;
	ID3D12GraphicsCommandList *const commandList;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;
	std::unique_ptr<Waves> waves;

	void buildGeometry(std::vector<GeometryGenerator::MeshData> meshes);
	SubmeshGeometry addSubmesh(GeometryGenerator::MeshData item, UINT itemVertexOffset, UINT itemIndexOffset);
	void buildWavesGeometryBuffers();

	float getHillsHeight(float x, float z)const;
	DirectX::XMFLOAT3 getHillsNormal(float x, float z)const;

	void buildSkullGeometry();
};


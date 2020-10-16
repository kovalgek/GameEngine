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

struct Vertex2
{
	Vertex2() = default;
	Vertex2(float x, float y, float z, float nx, float ny, float nz, float u, float v) :
		Pos(x, y, z),
		Normal(nx, ny, nz),
		TexC(u, v) {}

	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
};

#pragma once
class GeometryStorage
{
public:
	GeometryStorage(
		ID3D12Device *device,
		ID3D12GraphicsCommandList *commandList
	);
	~GeometryStorage();

	MeshGeometry *getGeometry(const std::string name);
	Waves* getWaves() { return waves.get(); }

	std::unordered_map<std::string, std::vector<std::string>> getGeometryNames();

	void createMeshGeometry(std::string name, std::vector<GeometryGenerator::MeshData> meshes);

private:
	ID3D12Device *device;
	ID3D12GraphicsCommandList *commandList;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;
	std::unique_ptr<Waves> waves;

	void buildGeometry(std::vector<GeometryGenerator::MeshData> meshes);
	SubmeshGeometry addSubmesh(GeometryGenerator::MeshData item, UINT itemVertexOffset, UINT itemIndexOffset);
	void buildWavesGeometryBuffers();

	float getHillsHeight(float x, float z)const;
	DirectX::XMFLOAT3 getHillsNormal(float x, float z)const;

	void buildSkullGeometry();
	void buildRoomGeometry();
};


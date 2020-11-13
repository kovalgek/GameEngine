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


	std::unordered_map<std::string, std::vector<std::string>> getGeometryNames();

	void createMeshGeometry(std::string name, std::vector<MeshData> meshes);
	void createMeshGeometry(std::string meshName, std::string submeshName, std::vector<std::uint16_t> indices, int vertexCount);

	MeshGeometry* getGeometry(const std::string name) const;

private:
	ID3D12Device *device;
	ID3D12GraphicsCommandList *commandList;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;

	SubmeshGeometry addSubmesh(MeshData item, UINT itemVertexOffset, UINT itemIndexOffset);
};


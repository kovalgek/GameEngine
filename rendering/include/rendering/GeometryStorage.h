#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <windows.h>
#include <DirectXMath.h>

#include <string>
#include <unordered_map>
#include <wrl.h>


struct MeshGeometry;
struct SubmeshGeometry;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct MeshData;

class GeometryStorage
{
public:
	GeometryStorage(
		ID3D12Device *device,
		ID3D12GraphicsCommandList *commandList
	);
	~GeometryStorage();

	// Creates MeshGeometry data that stores mesh data in GPU memory
	void createMeshGeometry(std::string name, std::vector<MeshData> meshes);
	void createMeshGeometry2(std::string name, std::vector<MeshData> meshes);
	void buildRoomGeometry();

	// Uses for dynamic vertices data
	void createMeshGeometry(std::string meshName, std::string submeshName, std::vector<std::uint16_t> indices, int vertexCount);

	MeshGeometry* getGeometry(const std::string name) const;
	std::unordered_map<std::string, std::vector<std::string>> getGeometryNames();


private:
	ID3D12Device *device;
	ID3D12GraphicsCommandList *commandList;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;

	SubmeshGeometry addSubmesh(MeshData item, UINT itemVertexOffset, UINT itemIndexOffset);
};
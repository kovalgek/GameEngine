#include "GeometryStorage.h"
#include "Vertex.h"
#include "MathHelper.h"
#include "d3dUtil.h"
#include "Waves.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3d12.h>
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <fstream>
#include <array>

using namespace DirectX;
using namespace DirectX::PackedVector;

GeometryStorage::GeometryStorage(
	ID3D12Device *device,
	ID3D12GraphicsCommandList *commandList
) :
	device { device },
	commandList { commandList }
{
	waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
}

GeometryStorage::~GeometryStorage() = default;

void GeometryStorage::createMeshGeometry(std::string name, std::vector<GeometryGenerator::MeshData> meshes)
{
	UINT currentVertexOffset = 0;
	UINT currentIndexOffset = 0;

	auto totalVertexCount = 0;
	std::vector<Vertex> vertices;
	std::vector<std::uint16_t> indices;
	UINT k = 0;

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = name;

	for (GeometryGenerator::MeshData mesh : meshes)
	{
		SubmeshGeometry submeshGeometry = addSubmesh(mesh, currentVertexOffset, currentIndexOffset);
		currentVertexOffset += (UINT)mesh.Vertices.size();
		currentIndexOffset += (UINT)mesh.Indices32.size();

		totalVertexCount += mesh.Vertices.size();

		for (size_t i = 0; i < mesh.Vertices.size(); ++i)
		{
			auto vertex = Vertex();
			auto& p = mesh.Vertices[i].Position;
			vertex.Pos = p;
			vertex.Normal = mesh.Vertices[i].Normal;
			vertex.TexC = mesh.Vertices[i].TexC;
			vertices.push_back(vertex);
		}

		indices.insert(indices.end(), std::begin(mesh.GetIndices16()), std::end(mesh.GetIndices16()));

		geo->DrawArgs[mesh.Name] = submeshGeometry;
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::createDefaultBuffer(device,
		commandList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::createDefaultBuffer(device,
		commandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geometries[geo->Name] = std::move(geo);
}

SubmeshGeometry GeometryStorage::addSubmesh(GeometryGenerator::MeshData item, UINT itemVertexOffset, UINT itemIndexOffset)
{
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)item.Indices32.size();
	boxSubmesh.StartIndexLocation = itemIndexOffset;
	boxSubmesh.BaseVertexLocation = itemVertexOffset;
	return boxSubmesh;
}

void GeometryStorage::buildWavesGeometryBuffers()
{
	waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	std::vector<std::uint16_t> indices(3 * waves->TriangleCount()); // 3 indices per face
	assert(waves->VertexCount() < 0x0000ffff);

	// Iterate over each quad.
	int m = waves->RowCount();
	int n = waves->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	UINT vbByteSize = waves->VertexCount() * sizeof(Vertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "waterGeo";

	// Set dynamically.
	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->IndexBufferGPU = d3dUtil::createDefaultBuffer(device,
		commandList, indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	geometries["waterGeo"] = std::move(geo);
}


MeshGeometry *GeometryStorage::getGeometry(const std::string name)
{
	return geometries[name].get();
}

std::unordered_map<std::string, std::vector<std::string>> GeometryStorage::getGeometryNames()
{
	std::unordered_map<std::string, std::vector<std::string>> names;

	for (auto& x : geometries)
	{
		std::string geomertyName = x.first;
		auto &meshGeomerty = x.second;
		auto drawArgs = meshGeomerty->DrawArgs;
		std::vector<std::string> args;

		for (auto& drawArg : drawArgs)
		{
			args.push_back(drawArg.first);
		}

		names[geomertyName] = args;
	}
	return names;
}

#include "GeometryStorage.h"
#include "MathHelper.h"
#include "d3dUtil.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3d12.h>
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include <fstream>
#include <array>

#include "MeshData.h"
#include "MeshVertex.h"
#include "Vertex.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

GeometryStorage::GeometryStorage(
	ID3D12Device *device,
	ID3D12GraphicsCommandList *commandList
) :
	device { device },
	commandList { commandList }
{
}

GeometryStorage::~GeometryStorage() = default;

void GeometryStorage::createMeshGeometry(std::string name, std::vector<MeshData> meshes)
{
	UINT currentVertexOffset = 0;
	UINT currentIndexOffset = 0;

	auto totalVertexCount = 0;
	std::vector<Vertex> vertices;
	std::vector<std::uint16_t> indices;
	UINT k = 0;

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = name;

	for (MeshData mesh : meshes)
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

SubmeshGeometry GeometryStorage::addSubmesh(MeshData item, UINT itemVertexOffset, UINT itemIndexOffset)
{
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)item.Indices32.size();
	boxSubmesh.StartIndexLocation = itemIndexOffset;
	boxSubmesh.BaseVertexLocation = itemVertexOffset;
	return boxSubmesh;
}

MeshGeometry *GeometryStorage::getGeometry(const std::string name) const
{
	return geometries.find(name)->second.get();
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

void GeometryStorage::createMeshGeometry(std::string meshName, std::string submeshName, std::vector<std::uint16_t> indices, int vertexCount)
{
	UINT vbByteSize = vertexCount * sizeof(Vertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = meshName;

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

	geo->DrawArgs[submeshName] = submesh;

	geometries[meshName] = std::move(geo);
}

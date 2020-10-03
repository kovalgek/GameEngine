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
	ID3D12Device* const device,
	ID3D12GraphicsCommandList* const commandList
) :
	device { device },
	commandList { commandList }
{
	waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	GeometryGenerator geoGen;

	std::vector<GeometryGenerator::MeshData> meshes;

	GeometryGenerator::MeshData box = geoGen.CreateBox(8.0f, 8.0f, 8.0f, 3);
	box.Name = "box";
	meshes.push_back(box);

	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	grid.Name = "grid";
	meshes.push_back(grid);

	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	sphere.Name = "sphere";
	meshes.push_back(sphere);

	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);
	cylinder.Name = "cylinder";
	meshes.push_back(cylinder);

	GeometryGenerator::MeshData hills = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);
	hills.Name = "hills";
	meshes.push_back(hills);

	//buildGeometry(meshes);

	//buildWavesGeometryBuffers();
	buildSkullGeometry();

	buildRoomGeometry();
}

GeometryStorage::~GeometryStorage() = default;

void GeometryStorage::buildGeometry(std::vector<GeometryGenerator::MeshData> meshes)
{
	UINT currentVertexOffset = 0;
	UINT currentIndexOffset = 0;

	auto totalVertexCount = 0;
	std::vector<Vertex> vertices;
	std::vector<std::uint16_t> indices;
	UINT k = 0;

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	for (GeometryGenerator::MeshData mesh : meshes)
	{
		SubmeshGeometry submeshGeometry = addSubmesh(mesh, currentVertexOffset, currentIndexOffset);
		currentVertexOffset += (UINT)mesh.Vertices.size();
		currentIndexOffset += (UINT)mesh.Indices32.size();

		totalVertexCount += mesh.Vertices.size();

		if (mesh.Name == "hills")
		{		
			for (size_t i = 0; i < mesh.Vertices.size(); ++i, ++k)
			{
				auto vertex = Vertex();

				auto& p = mesh.Vertices[i].Position;
				vertex.Pos = p;
				vertex.Pos.y = getHillsHeight(p.x, p.z);
				vertex.Normal = getHillsNormal(p.x, p.z);
				vertex.TexC = mesh.Vertices[i].TexC;

				vertices.push_back(vertex);
			}
		}
		else if (mesh.Name == "box")
		{
			for (size_t i = 0; i < mesh.Vertices.size(); ++i)
			{
				auto vertex = Vertex();
				auto& p = mesh.Vertices[i].Position;
				vertex.Pos = p;
				vertex.Normal = mesh.Vertices[i].Normal;
				vertex.TexC = mesh.Vertices[i].TexC;
				vertices.push_back(vertex);
			}
		}
		else
		{
			for (size_t i = 0; i < mesh.Vertices.size(); ++i, ++k)
			{
				auto vertex = Vertex();
				vertex.Pos = mesh.Vertices[i].Position;
				vertices.push_back(vertex);
			}
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

float GeometryStorage::getHillsHeight(float x, float z)const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 GeometryStorage::getHillsNormal(float x, float z)const
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
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

void GeometryStorage::buildSkullGeometry()
{
	std::ifstream fin("../core/resources/models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

		// Model does not have texture coordinates, so just zero them out.
		vertices[i].TexC = { 0.0f, 0.0f };
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	std::vector<std::int32_t> indices(3 * tcount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);

	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "skullGeo";

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
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["skull"] = submesh;

	geometries[geo->Name] = std::move(geo);
}

void GeometryStorage::buildRoomGeometry()
{
	// Create and specify geometry.  For this sample we draw a floor
// and a wall with a mirror on it.  We put the floor, wall, and
// mirror geometry in one vertex buffer.
//
//   |--------------|
//   |              |
//   |----|----|----|
//   |Wall|Mirr|Wall|
//   |    | or |    |
//   /--------------/
//  /   Floor      /
// /--------------/

	std::array<Vertex2, 20> vertices =
	{
		// Floor: Observe we tile texture coordinates.
		Vertex2(-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f), // 0 
		Vertex2(-3.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		Vertex2(7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f),
		Vertex2(7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f),

		// Wall: Observe we tile texture coordinates, and that we
		// leave a gap in the middle for the mirror.
		Vertex2(-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 4
		Vertex2(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Vertex2(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f),
		Vertex2(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f),

		Vertex2(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f), // 8 
		Vertex2(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Vertex2(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f),
		Vertex2(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f),

		Vertex2(-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 12
		Vertex2(-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Vertex2(7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f),
		Vertex2(7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f),

		// Mirror
		Vertex2(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f), // 16
		Vertex2(-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Vertex2(2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		Vertex2(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f)
	};

	std::array<std::int16_t, 30> indices =
	{
		// Floor
		0, 1, 2,
		0, 2, 3,

		// Walls
		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		// Mirror
		16, 17, 18,
		16, 18, 19
	};

	SubmeshGeometry floorSubmesh;
	floorSubmesh.IndexCount = 6;
	floorSubmesh.StartIndexLocation = 0;
	floorSubmesh.BaseVertexLocation = 0;

	SubmeshGeometry wallSubmesh;
	wallSubmesh.IndexCount = 18;
	wallSubmesh.StartIndexLocation = 6;
	wallSubmesh.BaseVertexLocation = 0;

	SubmeshGeometry mirrorSubmesh;
	mirrorSubmesh.IndexCount = 6;
	mirrorSubmesh.StartIndexLocation = 24;
	mirrorSubmesh.BaseVertexLocation = 0;

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "roomGeo";

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

	geo->DrawArgs["floor"] = floorSubmesh;
	geo->DrawArgs["wall"] = wallSubmesh;
	geo->DrawArgs["mirror"] = mirrorSubmesh;

	geometries[geo->Name] = std::move(geo);
}
#include "GeometryStorage.h"
#include "GeometryGenerator.h"
#include "Vertex.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3d12.h>
#include <DirectXColors.h>
#include <D3Dcompiler.h>
#include "MathHelper.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

GeometryStorage::GeometryStorage(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList,
	ID3D12CommandAllocator* commandAllocator,
	ID3D12CommandQueue* commandQueue) :
	device { device },
	commandList { commandList },
	commandAllocator { commandAllocator },
	commandQueue { commandQueue }
{
	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(commandList->Reset(commandAllocator, nullptr));

	buildShapeGeometry();

	// Execute the initialization commands.
	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* cmdsLists[] = { commandList };
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	//appContext.lock()->flushCommandQueue();
}

GeometryStorage::~GeometryStorage()
{

}

void GeometryStorage::buildShapeGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	// Define the SubmeshGeometry that cover different 
	// regions of the vertex/index buffers.

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::Crimson);
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::SteelBlue);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

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

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	geometries[geo->Name] = std::move(geo);
}

MeshGeometry *GeometryStorage::getGeometry(std::string name)
{
	return geometries[name].get();
}

//std::vector<std::unique_ptr<RenderItem>> GeometryStorage::renderItems()
//{
//	std::vector<std::unique_ptr<RenderItem>> allRitems;
//
//	auto boxRitem = std::make_unique<RenderItem>();
//	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
//	boxRitem->ObjCBIndex = 0;
//	boxRitem->Geo = geometries["shapeGeo"].get();
//	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
//	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
//	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
//	allRitems.push_back(std::move(boxRitem));
//
//	auto gridRitem = std::make_unique<RenderItem>();
//	gridRitem->World = MathHelper::Identity4x4();
//	gridRitem->ObjCBIndex = 1;
//	gridRitem->Geo = geometries["shapeGeo"].get();
//	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
//	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
//	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
//	allRitems.push_back(std::move(gridRitem));
//
//	UINT objCBIndex = 2;
//	for (int i = 0; i < 5; ++i)
//	{
//		auto leftCylRitem = std::make_unique<RenderItem>();
//		auto rightCylRitem = std::make_unique<RenderItem>();
//		auto leftSphereRitem = std::make_unique<RenderItem>();
//		auto rightSphereRitem = std::make_unique<RenderItem>();
//
//		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
//		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);
//
//		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
//		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);
//
//		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
//		leftCylRitem->ObjCBIndex = objCBIndex++;
//		leftCylRitem->Geo = geometries["shapeGeo"].get();
//		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
//		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
//		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
//
//		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
//		rightCylRitem->ObjCBIndex = objCBIndex++;
//		rightCylRitem->Geo = geometries["shapeGeo"].get();
//		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
//		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
//		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;
//
//		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
//		leftSphereRitem->ObjCBIndex = objCBIndex++;
//		leftSphereRitem->Geo = geometries["shapeGeo"].get();
//		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
//		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
//		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
//
//		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
//		rightSphereRitem->ObjCBIndex = objCBIndex++;
//		rightSphereRitem->Geo = geometries["shapeGeo"].get();
//		rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
//		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
//		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;
//
//		allRitems.push_back(std::move(leftCylRitem));
//		allRitems.push_back(std::move(rightCylRitem));
//		allRitems.push_back(std::move(leftSphereRitem));
//		allRitems.push_back(std::move(rightSphereRitem));
//	}
//
//	return allRitems;
//
//	//std::vector<RenderItem*> opaqueRitems;
//
//	//// All the render items are opaque.
//	//for (auto& e : allRitems)
//	//	opaqueRitems.push_back(e.get());
//
//	//return opaqueRitems;
//}

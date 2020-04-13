

#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "MathHelper.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include "UploadBuffer.h"
#include <vector>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

class ApplicationContext;
class GameTimer;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

#pragma once
class MainScene
{
public:
	//MainScene(const ComPtr<ID3D12Device>& device,
	//	const ComPtr<ID3D12CommandQueue>& commandQueue,
	//	const ComPtr<ID3D12CommandAllocator>& commandAllocator,
	//	const ComPtr<ID3D12GraphicsCommandList>& commandList);

	MainScene(ApplicationContext* appContext);
	void update(const GameTimer& gameTimer);
	void draw(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

private:
	void buildDescriptorHeaps();
	void buildConstantBuffers();
	void buildRootSignature();
	void buildShadersAndInputLayout();
	void buildBoxGeometry();
	void buildPSO();

	ComPtr<ID3D12PipelineState> PSO = nullptr;

	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> cbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	ApplicationContext* appContext;


	ComPtr<ID3DBlob> vsByteCode = nullptr;
	ComPtr<ID3DBlob> psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	std::unique_ptr<MeshGeometry> boxGeo = nullptr;


	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;


};


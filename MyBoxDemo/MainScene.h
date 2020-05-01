#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <memory>
#include "MathHelper.h"
#include "UploadBuffer.h"

class Application;
class GameTimer;

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

#pragma once
class MainScene
{
public:

	MainScene(Application* appContext);

	void update(const GameTimer& gameTimer);
	void draw(const GameTimer& gameTimer);
	void onResize(int clientWidth, int clientHeight);

	void onMouseDown(int x, int y);
	void onMouseUp(int x, int y);
	void onMouseMove(int x, int y);

private:
	Application* appContext;
	ID3D12Device* device;
	ID3D12CommandQueue* commandQueue;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilFormat;
	bool        msaa4xState;
	UINT        msaa4xQuality;

	void buildDescriptorHeaps();
	void buildConstantBuffers();
	void buildRootSignature();
	void buildShadersAndInputLayout();
	void buildBoxGeometry();
	void buildPSO();

	Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	
	Microsoft::WRL::ComPtr<ID3DBlob> vsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> psByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	std::unique_ptr<MeshGeometry> boxGeo = nullptr;
	
	DirectX::XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * DirectX::XM_PI;
	float mPhi = DirectX::XM_PIDIV4;
	float mRadius = 5.0f;

	POINT mLastMousePos;
};


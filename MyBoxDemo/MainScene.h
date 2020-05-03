#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <memory>
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "FrameResource.h"

class Application;
class GameTimer;


const int gNumFrameResources = 3;

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem
{
	RenderItem() = default;

	// World matrix of the shape that describes the object's local space
	// relative to the world space, which defines the position, orientation,
	// and scale of the object in the world.
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
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

	void updateCamera(const GameTimer& gameTimer);
	void buildShapeGeometry();
	void buildRenderItems();
	void buildFrameResources();

	void buildDescriptorHeaps();
	void buildConstantBufferViews();
	void buildRootSignature();
	void buildShadersAndInputLayout();
	void buildPSOs();

	void onKeyboardInput(const GameTimer& gameTimer);
	void updateObjectCBs(const GameTimer& gameTimer);
	void updateMainPassCB(const GameTimer& gameTimer);
	void drawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap = nullptr;


	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	std::unique_ptr<MeshGeometry> boxGeo = nullptr;
	
	DirectX::XMFLOAT3 eyePosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * DirectX::XM_PI;
	float mPhi = 0.2f * DirectX::XM_PI;
	float mRadius = 15.0f;

	POINT mLastMousePos;


	// new
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> shaders;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> geometries;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> allRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> opaqueRitems;

	UINT passCbvOffset = 0;

	UINT cbvSrvUavDescriptorSize = 0;

	bool isWireframe = false;

	PassConstants mainPassCB;
};


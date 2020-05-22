#include <unordered_map>
#include <wrl/client.h>
#include <vector>
#include <string>

struct ID3D12Device;
enum DXGI_FORMAT;
struct ID3D12PipelineState;
struct ID3D12RootSignature;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;
struct D3D12_INPUT_ELEMENT_DESC;

class PipleneStateData
{
public:
	PipleneStateData(
		ID3D12Device* device,
		DXGI_FORMAT backBufferFormat,
		DXGI_FORMAT depthStencilFormat,
		bool msaa4xState,
		UINT msaa4xQuality);

	ID3D12PipelineState* getPSO(std::string name);
	ID3D12RootSignature* getRootSignature() const { return rootSignature.Get(); }
private:
	void buildShadersAndInputLayout();
	void buildRootSignature();
	void buildPSOs();

	ID3D12Device* device;
	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilFormat;
	bool msaa4xState;
	UINT msaa4xQuality;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> mPSOs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> shaders;
};

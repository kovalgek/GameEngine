

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#define MAX_LOADSTRING 100


class ApplicationContext
{
public:
	ApplicationContext(HWND mainWindowHandle);
	~ApplicationContext();
	bool initialize();
	void onResize();

	void windowDidActivate();

	Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice;
	int clientWidth = 800;
	int clientHeight = 600;
private:

	ID3D12Resource* currentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView()const;

	void enableDebugModeIfNeeded();
	void createDevice();
	void createFence();
	void createDXGIFactory();
	void setUpDescriptorSizes();
	void setUp4xMSAAQuality();
	void createCommandObjects();
	void createSwapChain();
	void createRtvAndDsvDescriptorHeaps();
	void flushCommandQueue();

	void logAdapters();
	void logAdapterOutputs(IDXGIAdapter* adapter);
	void logOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	HINSTANCE applicationInstanceHandle  = nullptr;
	HWND mainWindowHandle = nullptr;
	WCHAR windowClass[MAX_LOADSTRING];            // the main window class name


	HINSTANCE mhAppInst = nullptr; // application instance handle
	



	// Set true to use 4X MSAA (§4.1.8).  The default is false.
	bool      msaa4xState = false;    // 4X MSAA enabled
	UINT      msaa4xQuality = 0;      // quality level of 4X MSAA

	// Used to keep track of the “delta-time” and game time (§4.4).
	//GameTimer mTimer;

	Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 currentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	static const int swapChainBufferCount = 2;
	int currBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainBuffer[swapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	D3D12_VIEWPORT screenViewport;
	D3D12_RECT scissorRect;

	UINT rtvDescriptorSize = 0;
	UINT dsvDescriptorSize = 0;
	UINT cbvSrvUavDescriptorSize = 0;

	// Derived class should set these in derived constructor to customize starting values.
	//std::wstring mMainWndCaption = L"d3d App";
	D3D_DRIVER_TYPE d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

};


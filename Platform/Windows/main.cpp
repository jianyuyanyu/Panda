// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <stdint.h>

#include <d3d12.h>
#include "d3dx12.h"
#include <DXGI1_4.h>		// DXGI (DirectX Graphics Infrastructure) is an API used along with Direct3D. The basic idea of 
							// DXGI is that some graphics related tasks are common to multipile graphics APIs. Here are some
							// common functionality, swap chain, display adapters, monitors and supported display modes(
							// resolution, refresh rate), surface formats
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

#include <wrl/client.h>

#include <string>
#include <exception>

namespace Panda {
	// Helper class for COM exceptions
	class com_exception : public std::exception {
	public:
		com_exception(HRESULT hr) : result(hr) {
		}
		
		virtual const char* what() const override {
			static char s_str[64] = {0};
			sprintf_s(s_str, "Failure with HRESULT of %08X", 
				static_cast<unsigned int>(result));
			return s_str;
		}
		
	private:
		HRESULT result;
	};
	
	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed (HRESULT hr) {
		if (FAILED(hr)) {
			throw com_exception(hr);
		}
	}
};

using namespace Panda;
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;
using namespace std;

ComPtr<IDXGIFactory4> 							g_pDXGIFactory = nullptr;			// dxgi factory
ComPtr<ID3D12Device>							g_pDevice = nullptr;				// the pointer to out Direct3D device interface
D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS	g_msQualityLevels;	// multi-sample support information

ComPtr<ID3D12CommandQueue>			g_pCommandQueue;				// the pointer to command queue
ComPtr<ID3D12CommandAllocator>		g_pCommandAllocator;			// the pointer to command buffer allocator
ComPtr<ID3D12GraphicsCommandList>	g_pCommandList;					// a list to store CPU commands, which will be submmited to GPU to execute

const uint32_t 	g_FrameCount = 2;			// buffer count
HWND 			g_hWnd;						// window handle

ComPtr<IDXGISwapChain> 				g_pSwapChain = nullptr;			// the pointer to the swap chain interface 
uint32_t g_ScreenWidth  =  960;
uint32_t g_ScreenHeight =  480;

ComPtr<ID3D12Fence>	g_pFence;	// the pointer to the fence
uint32_t g_rtvDescriptorSize = 0;
ComPtr<ID3D12DescriptorHeap> g_pRtvHeap;
ComPtr<ID3D12DescriptorHeap> g_pDsvHeap;

uint32_t g_CurrentFence = 0; // current fence value

uint32_t g_currentBackBuffer = 0;
ComPtr<ID3D12Resource> g_SwapChainBuffer[g_FrameCount];
ComPtr<ID3D12Resource> g_DepthStencilBuffer;


bool InitMainWindow(HINSTANCE hInstance, int nCmdShow) {	
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("HelloD3D12");
	
	// register the window class
    RegisterClassEx(&wc);
	
    // create the window and use the result as the handle
    g_hWnd = CreateWindowEx(0,
                          _T("HelloD3D12"),                   // name of the window class
                          _T("Hello Direct 12"),      			// title of the window
                          WS_OVERLAPPEDWINDOW,                  // window style
                          100,                                  // x-position of the window
                          100,                                  // y-position of the window
                          nScreenWidth,                         // width of the window
                          nScreenHeight,                        // height of the window
                          NULL,                                 // we have no parent window, NULL
                          NULL,                                 // we aren't using menus, NULL
                          hInstance,                            // application handle
                          NULL);                                // used with multiple windows, NULL

	if (!g_hWnd) {
		MessageBoxW(0, L"Create window failed.", 0, 0);
		return false;
	}
						  
    // display the window on the screen
    ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	
	return true;
}

/***************************************************************************************/
/*
 * DirectX 12 
 */
 
void CreateDevice() {
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,	// default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&g_pDevice));
	
	// Fallback to WARP(Windows Advanced Rasterization Platform) device
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&g_pDXGIFactory)));
	if (FAILED(hardwareResult)) {
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(g_pDXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		
		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&g_pDevice)));
	}
	
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render
	// target formats, so we only need to check quality support.
	g_msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	g_msQualityLevels.SampleCount = 4;
	g_msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	g_msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(g_pDevice->CheckFeatureSupport(
				D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
				&g_msQualityLevels,
				sizeof (g_msQualityLevels)));
}

void CreateCommandObjects() {
	// Command Queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(g_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_pCommandQueue)));
	
	// Command Allocator and Command List
	ThrowIfFailed(g_pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(g_pCommandAllocator.GetAddressOf())));
		
	ThrowIfFailed(g_pDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		g_pCommandAllocator.Get(),	// Assocated command allocator
		nullptr, // Initial PipelineStateObject
		IID_PPV_ARGS(g_pCommandList.GetAddressOf())));
		
	// Close command list. 
	g_pCommandList->Close();
}
 
void CreateSwapChain() {
	// Release the previous swapchain we will be recreating.
	g_pSwapChain.Reset();
	
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = g_ScreenWidth;
	sd.BufferDesc.Height = g_ScreenHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = g_msQualityLevels.NumQualityLevels - 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = g_FrameCount;
	sd.OutputWindow = g_hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	// Note: Swap chain uses queue to perform flush
	ThrowIfFailed(g_pDXGIFactory->CreateSwapChain(
					g_pCommandQueue.Get(),
					&sd,
					g_pSwapChain.GetAddressOf()));
}

void CreateDescriptors() {
	// fence
	ThrowIfFailed(g_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence)));
	
	// RTVs
	g_rtvDescriptorSize = g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = g_FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(g_pDevice->CreateDescriptorHeap(
					&rtvHeapDesc,
					IID_PPV_ARGS(g_pRtvHeap.GetAddressOf())));
					
	// DSVs
	g_dsvDescriptorSize = g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(g_pDevice->CreateDescriptorHeap(
					&dsvHeapDesc,
					IID_PPV_ARGS(g_pDsvHeap.GetAddressOf())));
}

void CreateBuffers() {
	// Release the previous resources we will be recreating.
	for (int i = 0; i < g_FrameCount; ++i)
		g_SwapChainBuffer[i].Reset();
	g_DepthStencilBuffer.Reset();
	
	// Resize the swap chain
	ThrowIfFailed(g_pSwapChain->ResizeBuffers(
		g_FrameCount,
		g_ScreenWidth, g_ScreenHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	
	g_currentBackBuffer = 0;
	
	// Create render target views.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(g_pRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < g_FrameCount; ++i) {
		ThrowIfFailed(g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_SwapChainBuffer[i])));
		g_pDevice->CreateRenderTargetView(g_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, g_rtvDescriptorSize);	
	}
	
	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = g_ScreenWidth;
	depthStencilDesc.Height = g_ScreenHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = g_msQualityLevels.SampleCount;
	depthStencilDesc.SampleDesc.Quality = g_msQualityLevels.NumQualityLevels - 1;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
	
}

void FlushCommandQueue() {
	// Advance the fence value to mark commands up to this fence point.
	g_CurrentFence++;
	
	// Add an instruction to the command queue to set a new fence point. Because we
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(g_pCommandQueue->Signal(g_pFence.Get(), g_CurrentFence));
	
	// Wait until the GPU has completed commands up to this fence point.
	if (g_pFence->GetCompletedValue() < g_CurrentFence) {
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		
		// Set fire event when GPU hits current fence.
		ThrowIfFailed(g_pFence->SetEventOnCompletion(g_CurrentFence, eventHandle));
		
		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void OnResize() {
	// We won't resize it until the device,swap chain, command allocator are created.
	if (g_pDevice == nullptr || g_pSwapChain == nullptr || g_pCommandAllocator == nullptr) {
		return;
	}
	
	// Fluse before changing any resources.
	FlushCommandQueue();
	
	
}

void InitDirect3D12() {
	CreateDevice();
	
	// We need them here?
	// ThrowIfFailed(g_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence)));
	// mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	CreateCommandObjects();
	
	CreateSwapChain();
	
	CreateBuffers();
	
	CreateDescriptors();
}

void Draw() {
}

/***************************************************************************************/

int Run() {
    // this struct holds Windows event messages
    MSG msg;

    // wait for the next message in the queue, store the result in 'msg'
	while(msg.message != WM_QUIT) {
		// If there are window message then process them
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);
		}
		else {
			Draw();
		}
	}

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR lpCmdLine,
                   int nCmdShow)
{
	// Create window
	InitMainWindow(hInstance, nCmdShow);
	
	// Initialize DirectX 12
	InitDirect3D12();
	
	return Run();
}
